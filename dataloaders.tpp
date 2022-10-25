#ifndef SFM_DATALOADERS_H
#define SFM_DATALOADERS_H

#include <torch/torch.h>
#include "net.h"

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

#endif
