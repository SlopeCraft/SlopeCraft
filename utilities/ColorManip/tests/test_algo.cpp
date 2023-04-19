#include <CLI11.hpp>
#include <ColorManip.h>
#include <Eigen/Dense>
#include <GPU_interface.h>
#include <SC_GlobalEnums.h>
#include <iostream>
#include <omp.h>
#include <random>

using std::cout, std::endl;

struct task_t {
  std::vector<std::array<float, 3>> colorset_c3;
  std::vector<std::array<float, 3>> task_c3;

  const uint32_t platidx;
  const uint32_t devidx;
  const SCL_convertAlgo algo;
};

int list_gpu() noexcept;

int run_task(task_t &) noexcept;

int main(int argc, char **argv) {
  CLI::App app;

  bool list_gpu{false};

  uint32_t platidx{0}, devidx{0};

  size_t colorset_size{0}, task_size{0};

  char algo = 'r';

  app.add_flag("--list", list_gpu)->default_val(false);
  app.add_option("--platform-idx", platidx)
      ->default_val(0)
      ->check(CLI::NonNegativeNumber);
  app.add_option("--device-idx", devidx)
      ->default_val(0)
      ->check(CLI::NonNegativeNumber);

  app.add_option("--colorset-size", colorset_size)
      ->default_val(1 << 15)
      ->check(CLI::Range({1, UINT16_MAX - 1}));

  app.add_option("--task-size", task_size)
      ->default_val(1 << 15)
      ->check(CLI::PositiveNumber);

  app.add_option("--algo", algo)
      ->default_val('r')
      ->check(CLI::IsMember({'r', 'R', 'H', 'l', 'L', 'X'}));

  CLI11_PARSE(app, argc, argv);

  if (list_gpu) {
    return ::list_gpu();
  }

  task_t task{{}, {}, platidx, devidx, SCL_convertAlgo(algo)};

  task.colorset_c3.resize(colorset_size);
  task.task_c3.resize(task_size);

  return run_task(task);
}

int list_gpu() noexcept {
  size_t num_plats = gpu_wrapper::platform_num();
  cout << num_plats << " platforms found." << endl;
  for (size_t idx_plat = 0; idx_plat < num_plats; idx_plat++) {
    gpu_wrapper::platform_wrapper *pw =
        gpu_wrapper::platform_wrapper::create(idx_plat);

    const size_t num_devs = pw->num_devices_v();
    cout << "Platform " << idx_plat << " : " << pw->name_v() << " has "
         << num_devs << " devices : \n";

    for (size_t devidx = 0; devidx < num_devs; devidx++) {
      gpu_wrapper::device_wrapper *dw =
          gpu_wrapper::device_wrapper::create(pw, devidx);
      cout << "    " << devidx << " : " << dw->name_v() << '\n';
      gpu_wrapper::device_wrapper::destroy(dw);
    }

    gpu_wrapper::platform_wrapper::destroy(pw);
  }

  cout << endl;
  return 0;
}

void fill_c3arr(std::vector<std::array<float, 3>> &dst, std::mt19937 &mt,
                SCL_convertAlgo algo) noexcept {
  std::uniform_real_distribution<float> randf(0, 1);
  for (auto &c3 : dst) {
    for (float &val : c3) {
      val = randf(mt);
    }

    switch (algo) {
      case SCL_convertAlgo::RGB:
      case SCL_convertAlgo::RGB_Better:
        break;
      case SCL_convertAlgo::HSV:
        RGB2HSV(c3[0], c3[1], c3[2], c3[0], c3[1], c3[2]);
        break;
      case SCL_convertAlgo::Lab00:
      case SCL_convertAlgo::Lab94:
        RGB2XYZ(c3[0], c3[1], c3[2], c3[0], c3[1], c3[2]);
        XYZ2Lab(c3[0], c3[1], c3[2], c3[0], c3[1], c3[2]);
        break;
      case SCL_convertAlgo::XYZ:
        RGB2XYZ(c3[0], c3[1], c3[2], c3[0], c3[1], c3[2]);
        break;
      default:
        abort();
        break;
    }
  }
}

#define HANDLE_ERR(gi, ret)                                              \
  if (gi == nullptr) {                                                   \
    cout << "failed to create gpu resource handle. gi==nullptr" << endl; \
    return ret;                                                          \
  }                                                                      \
  if (!gi->ok_v()) {                                                     \
    cout << gi->error_detail_v() << " : " << gi->error_code_v() << endl; \
    return ret;                                                          \
  }

int run_task(task_t &task) noexcept {
  std::random_device rd;
  std::mt19937 mt(rd());

  fill_c3arr(task.task_c3, mt, task.algo);
  fill_c3arr(task.colorset_c3, mt, task.algo);

  Eigen::ArrayXXf eig_colorset;
  {
    Eigen::Map<const Eigen::ArrayXXf> map_colorset(
        task.colorset_c3.data()->data(), 3, task.colorset_c3.size());

    eig_colorset = map_colorset.transpose();
  }

  if (gpu_wrapper::platform_num() <= 0) {
    cout << "No avaliable opencl platforms." << endl;
    return 0;
  }

  auto plat = gpu_wrapper::platform_wrapper::create(task.platidx);

  auto dev = gpu_wrapper::device_wrapper::create(plat, task.devidx);

  gpu_wrapper::gpu_interface *const gi =
      gpu_wrapper::gpu_interface::create(plat, dev);
  HANDLE_ERR(gi, 1);

  gpu_wrapper::device_wrapper::destroy(dev);
  gpu_wrapper::platform_wrapper::destroy(plat);

  gi->set_colorset_v(
      task.colorset_c3.size(),
      {&eig_colorset(0, 0), &eig_colorset(0, 1), &eig_colorset(0, 2)});
  HANDLE_ERR(gi, 2);

  gi->set_task_v(task.task_c3.size(), task.task_c3.data());
  HANDLE_ERR(gi, 3);

  double wtime = omp_get_wtime();
  gi->execute_v(task.algo, true);
  HANDLE_ERR(gi, 4);
  wtime = omp_get_wtime() - wtime;
  cout << "GPU finished in " << wtime * 1e3 << " ms" << endl;

  int ret = 0;
  for (size_t tid = 0; tid < task.task_c3.size(); tid++) {
    if (gi->result_idx_v()[tid] >= task.colorset_c3.size()) {
      cout << "Error : task " << tid << " has invalid result_idx "
           << gi->result_idx_v()[tid] << endl;
      ret = 5;
    }
  }

  gpu_wrapper::gpu_interface::destroy(gi);

  cout << "Success" << endl;

  return ret;
}