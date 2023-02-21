#include <CLI11.hpp>
#include <ColorDiff_OpenCL.h>
#include <ColorManip.h>
#include <Eigen/Dense>
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
  size_t num_plats = ocl_warpper::platform_num();
  cout << num_plats << " platforms found." << endl;
  for (size_t idx_plat = 0; idx_plat < num_plats; idx_plat++) {
    const size_t num_devs = ocl_warpper::device_num(idx_plat);
    cout << "Platform " << idx_plat << " : "
         << ocl_warpper::platform_str(idx_plat) << " has " << num_devs
         << " devices : \n";

    for (size_t devidx = 0; devidx < num_devs; devidx++) {
      cout << "    " << devidx << " : "
           << ocl_warpper::device_str(idx_plat, devidx) << '\n';
    }
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

#define HANDLE_ERR(rcs, ret)                                                   \
  if (!rcs.ok()) {                                                             \
    cout << rcs.error_detail() << " : " << rcs.error_code() << endl;           \
    return ret;                                                                \
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

  ocl_warpper::ocl_resource rcs(task.platidx, task.devidx);
  HANDLE_ERR(rcs, 1);

  rcs.set_colorset(
      task.colorset_c3.size(),
      {&eig_colorset(0, 0), &eig_colorset(0, 1), &eig_colorset(0, 2)});
  HANDLE_ERR(rcs, 2);

  rcs.set_task(task.task_c3.data(), task.task_c3.size());
  HANDLE_ERR(rcs, 3);

  double wtime = omp_get_wtime();
  rcs.execute(task.algo, true);
  HANDLE_ERR(rcs, 4);
  wtime = omp_get_wtime() - wtime;
  cout << "GPU finished in " << wtime * 1e3 << " ms" << endl;

  for (size_t tid = 0; tid < task.task_c3.size(); tid++) {
    if (rcs.result_idx()[tid] >= task.colorset_c3.size()) {
      cout << "Error : task " << tid << " has invalid result_idx "
           << rcs.result_idx()[tid] << endl;
      return 5;
    }
  }

  cout << "Success" << endl;

  return 0;
}