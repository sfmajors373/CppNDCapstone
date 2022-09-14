#include <torch/torch.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

// Where to find dataset
const char* dataRoot = "./data";

// Batch size training
const int64_t trainBatchSize = 128;

// Batch size testing
const int64_t testBatchSize = 1000;

// num epochs
const int64_t numEpochs = 5;

// logging
const int64_t loggingInterval = 10;

struct Net : torch::nn::Module
{
    Net()
      // Conv2d model(Conv2dOptions(3, 2, 3).stride(1).bias(false));
      // ConvOptions(int_64_t in_channels, int64_t out_channels, kernel_size);
      :  conv1(torch::nn::Conv2dOptions(1, 6, /*kernel_size=*/5).padding(2)),
         conv2(torch::nn::Conv2dOptions(6, 16, /*kernel_size=*/5)),
         fc1(400,120),
         fc2(120,84),
         fc3(84,10)
  {
      register_module("conv1", conv1);
      register_module("conv2", conv2);
      register_module("fc1", fc1);
      register_module("fc2", fc2);
      register_module("fc3", fc3);

  }

  torch::Tensor forward(torch::Tensor x)
  {
      x = torch::sigmoid(conv1->forward(x));
      x = torch::avg_pool2d(x, 2);
      x = torch::sigmoid(conv2->forward(x));
      x = torch::avg_pool2d(x, 2);
      x = x.view({-1, 400});
      x = torch::sigmoid(fc1->forward(x));
      x = torch::sigmoid(fc2->forward(x));
      x = torch::sigmoid(fc3->forward(x));
      return torch::log_softmax(x, 1);
  }

  torch::nn::Conv2d conv1;
  torch::nn::Conv2d conv2;
  torch::nn::Linear fc1;
  torch::nn::Linear fc2;
  torch::nn::Linear fc3;
};

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
