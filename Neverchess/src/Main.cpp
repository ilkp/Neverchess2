
#include <iostream>
#include "TrainingManager.h"

int main()
{
	srand(time(NULL));
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point batchBegin;
	std::chrono::steady_clock::time_point batchEnd;
	std::chrono::steady_clock::time_point end;
	const int batchSize = 1;
	const int totalGames = 32768;

	simpleANN::CreateInfo annCreateInfo;
	annCreateInfo._inputSize = nvc::ANN_INPUT_LENGTH;
	annCreateInfo._numberOfHiddenLayers = 5;
	annCreateInfo._hiddenSize = 500;
	annCreateInfo._outputSize = 1;

	nvc::TrainingManagerCreateInfo tmCreateInfo;
	tmCreateInfo._nGames = batchSize;
	tmCreateInfo._nMaxMoves = 50;
	tmCreateInfo._nRandomMoves = 4;
	tmCreateInfo._trainSampleRate = 5;

	simpleANN::ANNetwork ann(annCreateInfo);
	nvc::TrainingManager tm(tmCreateInfo);

	begin = std::chrono::steady_clock::now();
	for (int i = 0; i < totalGames; i += batchSize)
	{
		batchBegin = std::chrono::steady_clock::now();
		tm.generateGames(ann);
		tm.train(ann);
		batchEnd = std::chrono::steady_clock::now();
		std::cout << "Batch finished - Played Games: " << i + batchSize
			<< " - Elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(batchEnd - batchBegin).count() << "ms"
			<< " - Total time: " << std::chrono::duration_cast<std::chrono::seconds>(batchEnd - begin).count() << "s"
			<< std::endl;
	}
	end = std::chrono::steady_clock::now();
	std::cout << "All batched finished - Total time: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "s" << std::endl;
	tm.exportANN(ann, std::to_string(totalGames));
	int exit;
	std::cin >> exit;
}
