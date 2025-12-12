
#pragma once

#include <functional>
#include <optional>
#include <ostream>
#include <vector>

#include "InputVector.hpp"
#include "Matrix.hpp"

namespace lms::som
{
    using LearningFactor = InputVector::value_type;
    using Norm = InputVector::value_type;

    void checkSameDimensions(const InputVector& a, const InputVector& b);
    void checkSameDimensions(const InputVector& a, std::size_t inputDimCount);
    std::ostream& operator<<(std::ostream& os, const InputVector& a);

    class Network
    {
    public:
        // Init a network with random values
        Network(Coordinate width, Coordinate height, std::size_t inputDimCount);

        Coordinate getWidth() const { return _refVectors.getWidth(); }
        Coordinate getHeight() const { return _refVectors.getHeight(); }
        std::size_t getInputDimCount() const { return _inputDimCount; }
        const InputVector& getDataWeights() const { return _weights; }

        // Set weight for each dimension (default is 1 for each weight)
        void setDataWeights(const InputVector& weights);

        // use this to manually construct a network without training
        void setRefVector(const Position& position, const InputVector& data);

        // <!> data must be normalized
        struct CurrentIteration
        {
            std::size_t idIteration;
            std::size_t iterationCount;
        };
        using ProgressCallback = std::function<void(const CurrentIteration&)>;
        using RequestStopCallback = std::function<bool()>;
        void train(const std::vector<InputVector>& dataSamples, std::size_t nbIterations, ProgressCallback = ProgressCallback{}, RequestStopCallback = RequestStopCallback{});

        const InputVector& getRefVector(const Position& position) const;
        Position getClosestRefVectorPosition(const InputVector& data) const;
        std::optional<Position> getClosestRefVectorPosition(const InputVector& data, InputVector::Distance maxDistance) const;

        std::optional<Position> getClosestRefVectorPosition(const std::vector<Position>& refVectorsPosition, InputVector::Distance maxDistance) const;

        InputVector::Distance getRefVectorsDistance(const Position& position1, const Position& position2) const;

        InputVector::Distance computeRefVectorsDistanceMean() const;
        InputVector::Distance computeRefVectorsDistanceMedian() const;

        void dump(std::ostream& os) const;

        // For each ref vector, update formula is:
        // i is the current iteration
        // refVector(i+1) = refVector(i) + LearningFactor(i) * NeighbourhoodFunc(i) * (MatchingRefVector - refVector)

        using DistanceFunc = std::function<InputVector::Distance(const InputVector& /* a */, const InputVector& /* b */, const InputVector& /* weights */)>;
        void setDistanceFunc(DistanceFunc distanceFunc);
        DistanceFunc getDistanceFunc() { return _distanceFunc; }

        using LearningFactorFunc = std::function<LearningFactor(const CurrentIteration&)>;
        void setLearningFactorFunc(LearningFactorFunc learningFactorFunc);

        using NeighbourhoodFunc = std::function<InputVector::value_type(Norm /* norm(Position - CoordMatchingRefVector) */, const CurrentIteration&)>;
        void setNeighbourhoodFunc(NeighbourhoodFunc neighbourhoodFunc);

    private:
        void updateRefVectors(const Position& closestRefVectorPosition, const InputVector& input, LearningFactor learningFactor, const CurrentIteration& iteration);

        std::size_t _inputDimCount{};
        InputVector _weights; // weight for each dimension
        Matrix<InputVector> _refVectors;

        DistanceFunc _distanceFunc;
        LearningFactorFunc _learningFactorFunc;
        NeighbourhoodFunc _neighbourhoodFunc;
    };
} // namespace lms::som
