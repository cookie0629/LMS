#include "database/objects/Image.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Directory.hpp"
#include "traits/PathTraits.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::Image)

namespace lms::db
{
    Image::Image(const std::filesystem::path& p)
    {
        setAbsoluteFilePath(p);
    }

    Image::pointer Image::create(Session& session, const std::filesystem::path& p)
    {
        session.checkWriteTransaction();
        auto image = std::unique_ptr<Image>(new Image(p));
        auto dboPtr = session.getDboSession()->add(std::move(image));
        return pointer(dboPtr);
    }

    std::size_t Image::getCount(Session& session)
    {
        session.checkReadTransaction();
        return session.getDboSession()->find<Image>().resultList().size();
    }

    Image::pointer Image::find(Session& session, ImageId id)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<Image>()
                          .where("id = ?")
                          .bind(id.getValue())
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    Image::pointer Image::find(Session& session, const std::filesystem::path& file)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<Image>()
                          .where("absolute_file_path = ?")
                          .bind(file.string())
                          .limit(1)
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    void Image::find(Session& session, const FindParameters& params, const std::function<void(const pointer&)>& func)
    {
        session.checkReadTransaction();
        auto query = session.getDboSession()->find<Image>();

        if (params.directory.getValue() != 0)
        {
            query.where("directory_id = ?").bind(params.directory.getValue());
        }

        if (!params.fileStem.empty())
        {
            query.where("stem = ?").bind(params.fileStem);
        }

        if (params.range)
        {
            query.offset(params.range->offset).limit(params.range->size);
        }

        auto results = query.resultList();
        for (auto& image : results)
        {
            func(pointer(image));
        }
    }

    void Image::setAbsoluteFilePath(const std::filesystem::path& path)
    {
        if (path.is_absolute())
        {
            _fileAbsolutePath = path;
            _fileStem = path.stem().string();
        }
    }

    ObjectPtr<Directory> Image::getDirectory() const
    {
        if (!_directory)
        {
            return {};
        }
        return ObjectPtr<Directory>(_directory);
    }

    void Image::setDirectory(ObjectPtr<Directory> directory)
    {
        _directory = getDboPtr(directory);
    }
} // namespace lms::db

