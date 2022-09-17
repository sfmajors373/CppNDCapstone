#include <cstddef>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "net.h"

// Where to find dataset
const char* dataRoot = "./data";

// Batch size training
const int64_t trainBatchSize = 128;

// Batch size testing
const int64_t testBatchSize = 1000;

// num epochs
const int64_t numEpochs = 10;

// logging
const int64_t loggingInterval = 10;

template <typename DataLoader>
void train(
  size_t epoch,
  Net& model,
  torch::Device device,
  DataLoader& data_loader,
  torch::optim::Optimizer& optimizer,
  size_t dataset_size
  )
{
    model.train();
    size_t batch_idx = 0;
    for (auto& batch : data_loader)
    {
        auto data = batch.data.to(device), targets = batch.target.to(device);
        optimizer.zero_grad();
        auto output = model.forward(data);
        auto loss = torch::nll_loss(output, targets);
        AT_ASSERT(!std::isnan(loss.template item<float>()));
        loss.backward();
        optimizer.step();

        if (batch_idx++ % loggingInterval == 0)
        {
            std::printf(
                  "\rTrain Epoch: %ld [%5ld/%5ld] Loss: %.4f",
                  epoch,
                  batch_idx * batch.data.size(0),
                  dataset_size,
                  loss.template item<float>()
                  );
        }
    }
}

template <typename DataLoader>
void test(
    Net& model,
    torch::Device device,
    DataLoader& data_loader,
    size_t dataset_size
    )
{
    torch::NoGradGuard no_grad;
    model.eval();
    double test_loss = 0;
    int32_t correct = 0;
    for (const auto& batch : data_loader)
    {
        auto data = batch.data.to(device), targets = batch.target.to(device);
        auto output = model.forward(data);
        test_loss += torch::nll_loss(
              output,
              targets,
              /*weight=*/{},
              torch::Reduction::Sum
            ).template item<float>();
        auto pred = output.argmax(1);
        // std::cout << "Pred: " << pred << "\n" << std::endl;
        correct += pred.eq(targets).sum().template item<int64_t>();
    }

    test_loss /= dataset_size;
    std::printf
      (
        "\nTest set: Average loss: %.4f | Accuracy: %.3f\n",
        test_loss,
        static_cast<double>(correct) / dataset_size
      );
}

int main()
{
  // figure out device and name it
  torch::DeviceType device_type;
  if (torch::cuda::is_available())
  {
      std::cout << "CUDA available!  Training on GPU." << std::endl;
      device_type = torch::kCUDA;
  } else {
      std::cout << "Training on CPU." << std::endl;
      device_type = torch::kCPU;
  }
  torch::Device device(device_type);

  Net model;
  model.to(device);

  // Training data
  auto train_dataset = torch::data::datasets::MNIST(dataRoot)
    .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
    .map(torch::data::transforms::Stack<>());

  const size_t train_dataset_size = train_dataset.size().value();

  auto train_loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(std::move(train_dataset), trainBatchSize);

  // Test data
  auto test_dataset = torch::data::datasets::MNIST(
      dataRoot, torch::data::datasets::MNIST::Mode::kTest)
    .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
    .map(torch::data::transforms::Stack<>());

  const size_t test_dataset_size = test_dataset.size().value();

  auto test_loader = torch::data::make_data_loader(std::move(test_dataset), testBatchSize);

  torch::optim::SGD optimizer(model.parameters(), torch::optim::SGDOptions(0.01).momentum(0.5));

  for (size_t epoch = 1; epoch <= numEpochs; ++epoch)
  {
      train(epoch, model, device, *train_loader, optimizer, train_dataset_size);
      test(model, device, *test_loader, test_dataset_size);
  }

    return 0;
}
