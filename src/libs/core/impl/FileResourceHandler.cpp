
#include "FileResourceHandler.hpp"

#include <algorithm>

#include "core/ILogger.hpp"
#include "core/MimeTypes.hpp"

namespace lms::core
{
    std::unique_ptr<IResourceHandler> createFileResourceHandler(const std::filesystem::path& path, std::string_view mimeType)
    {
        return std::make_unique<FileResourceHandler>(path, mimeType.empty() ? getMimeType(path.extension()) : mimeType);
    }

    FileResourceHandler::FileResourceHandler(const std::filesystem::path& path, std::string_view mimeType)
        : _mimeType{ mimeType }
        , _ifs{ path, std::ios::in | std::ios::binary }
    {
        if (!_ifs)
            LMS_LOG(UTILS, ERROR, "Cannot open file stream for " << path);
        else
        {
            _ifs.seekg(0, std::ios::end);
            if (!_ifs.fail())
                _fileSize = static_cast<::uint64_t>(_ifs.tellg());
            LMS_LOG(UTILS, DEBUG, "File " << path << ", fileSize = " << _fileSize);
        }
    }

    Wt::Http::ResponseContinuation* FileResourceHandler::processRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        if (_offset == 0)
        {
            if (!_ifs)
            {
                response.setStatus(404);
                return {};
            }

            response.addHeader("Accept-Ranges", "bytes");

            const Wt::Http::Request::ByteRangeSpecifier ranges{ request.getRanges(_fileSize) };
            if (!ranges.isSatisfiable())
            {
                std::ostringstream contentRange;
                contentRange << "bytes */" << _fileSize;
                response.setStatus(416); // Requested range not satisfiable
                response.addHeader("Content-Range", contentRange.str());

                LMS_LOG(UTILS, DEBUG, "Range not satisfiable");
                return {};
            }

            if (ranges.size() == 1)
            {
                LMS_LOG(UTILS, DEBUG, "Range requested = " << ranges[0].firstByte() << "-" << ranges[0].lastByte());

                response.setStatus(206);
                _offset = ranges[0].firstByte();
                _beyondLastByte = ranges[0].lastByte() + 1;

                std::ostringstream contentRange;
                contentRange << "bytes " << _offset << "-"
                             << _beyondLastByte - 1 << "/" << _fileSize;

                response.addHeader("Content-Range", contentRange.str());
                response.setContentLength(_beyondLastByte - _offset);
            }
            else
            {
                LMS_LOG(UTILS, DEBUG, "No range requested");

                response.setStatus(200);
                _beyondLastByte = _fileSize;
                response.setContentLength(_beyondLastByte);
            }

            LMS_LOG(UTILS, DEBUG, "Mimetype set to '" << _mimeType << "'");
            response.setMimeType(_mimeType);

            _ifs.seekg(static_cast<std::istream::pos_type>(_offset));
        } // end initial response setup

        ::uint64_t restSize = _beyondLastByte - _offset;
        ::uint64_t pieceSize = std::min(restSize, static_cast<::uint64_t>(_chunkSize));

        std::vector<char> buf(pieceSize);

        _ifs.read(buf.data(), buf.size());
        const ::uint64_t actualPieceSize{ static_cast<::uint64_t>(_ifs.gcount()) };
        if (actualPieceSize > 0)
        {
            response.out().write(buf.data(), actualPieceSize);
            LMS_LOG(UTILS, DEBUG, "Written " << actualPieceSize << " bytes, range = " << _offset << "-" << _offset + actualPieceSize - 1 << "");
        }
        else
            LMS_LOG(UTILS, DEBUG, "Written 0 byte");

        if (!_ifs.good())
            LMS_LOG(UTILS, WARNING, "Error reading from file!");
        else if (actualPieceSize < restSize)
        {
            _offset += actualPieceSize;
            LMS_LOG(UTILS, DEBUG, "Job not complete! Remaining range: " << _offset << "-" << _beyondLastByte - 1);

            return response.createContinuation();
        }

        LMS_LOG(UTILS, DEBUG, "Job complete!");
        return nullptr;
    }
} // namespace lms::core