#include <iostream>
#include <random>
#include <GPU_interface.h>

#include <omp.h>

using std::cout, std::endl;

std::random_device rd;
std::mt19937 mt(rd());

std::uniform_real_distribution<float> rand_f32(0, 1);
// std::uniform_int_distribution<uint32_t> rand_u32(0, UINT32_MAX);

int main(int, char **) {
  std::unique_ptr<gpu_wrapper::platform_wrapper> plat{
      gpu_wrapper::platform_wrapper::create(0)};
  std::unique_ptr<gpu_wrapper::device_wrapper> dev{
      gpu_wrapper::device_wrapper::create(plat.get(), 0)};

  gpu_wrapper::unique_gpu_interface gi{
      gpu_wrapper::gpu_interface::create(plat.get(), dev.get())};

  if (!gi->ok_v()) {
    cout << gi->error_code_v() << " : " << gi->error_detail_v() << endl;
    return 1;
  }

  cout << "Vendor = " << gi->device_vendor_v() << endl;

  const size_t colorset_size = 128;
  const size_t task_size = 256;

  const SCL_convertAlgo algo = SCL_convertAlgo::RGB;

  std::vector<float> colorset_R(colorset_size), colorset_B(colorset_size),
      colorset_G(colorset_size);
  {
    for (size_t cid = 0; cid < colorset_size; cid++) {
      colorset_R[cid] = rand_f32(mt);
      colorset_G[cid] = rand_f32(mt);
      colorset_B[cid] = rand_f32(mt);
    }
    // set colorset
    gi->set_colorset_v(colorset_size, {colorset_R.data(), colorset_G.data(),
                                       colorset_B.data()});
    if (!gi->ok_v()) {
      cout << gi->error_code_v() << " : " << gi->error_detail_v() << endl;
      return 3;
    }
  }
  cout << "colorset set." << endl;
  std::vector<std::array<float, 3>> tasks(task_size);
  {
    // initialize task with random colors
    for (size_t tid = 0; tid < task_size; tid++) {
      for (size_t cid = 0; cid < 3; cid++) {

        tasks[tid][cid] = rand_f32(mt);
      }
    }
    // set tasks
    gi->set_task_v(tasks.size(), tasks.data());
    if (!gi->ok_v()) {
      cout << gi->error_code_v() << " : " << gi->error_detail_v() << endl;
      return 2;
    }
  }

  cout << "tasks set." << endl;

  double wtime = omp_get_wtime();

  gi->execute_v(algo, true);
  if (!gi->ok_v()) {
    cout << gi->error_code_v() << " : " << gi->error_detail_v() << endl;
    return 4;
  }
  wtime = omp_get_wtime() - wtime;

  cout << "GPU finished in " << wtime * 1e3 << " ms" << endl;
  if (false) {
    for (size_t tid = 0; tid < task_size; tid++) {
      const std::array<float, 3> unconverted_color = tasks[tid];
      const uint16_t gpu_result_idx = gi->result_idx_v()[tid];
      const float gpu_result_diff = gi->result_diff_v()[tid];

      const std::array<float, 3> gpu_result_color = {
          colorset_R[gpu_result_idx], colorset_G[gpu_result_idx],
          colorset_B[gpu_result_idx]};

      float cpu_result_diff = 0;
      for (int c = 0; c < 3; c++) {
        cpu_result_diff += (unconverted_color[c] - gpu_result_color[c]) *
                           (unconverted_color[c] - gpu_result_color[c]);
      }

      cout << "task " << tid << ", gpu result diff = " << gpu_result_diff
           << ", cpu result diff = " << cpu_result_diff << '\n';
    }

    cout << "Success" << endl;
  }


  return 0;
}