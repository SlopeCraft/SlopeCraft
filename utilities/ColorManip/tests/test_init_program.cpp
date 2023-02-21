#include <ColorDiff_OpenCL.h>
#include <iostream>
#include <random>

#include <omp.h>

using std::cout, std::endl;

std::random_device rd;
std::mt19937 mt(rd());

std::uniform_real_distribution<float> rand_f32(0, 1);
// std::uniform_int_distribution<uint32_t> rand_u32(0, UINT32_MAX);

int main(int, char **) {
  ocl_warpper::ocl_resource rcs(0, 0);
  if (!rcs.ok()) {
    cout << rcs.error_code() << " : " << rcs.error_detail() << endl;
    return 1;
  }

  cout << "Vendor = " << rcs.device_vendor() << endl;

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
    rcs.set_colorset(colorset_size,
                     {colorset_R.data(), colorset_G.data(), colorset_B.data()});
    if (!rcs.ok()) {
      cout << rcs.error_code() << " : " << rcs.error_detail() << endl;
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
    rcs.set_task(tasks.size(), tasks.data());
    if (!rcs.ok()) {
      cout << rcs.error_code() << " : " << rcs.error_detail() << endl;
      return 2;
    }
  }

  cout << "tasks set." << endl;

  double wtime = omp_get_wtime();

  rcs.execute(algo, true);
  if (!rcs.ok()) {
    cout << rcs.error_code() << " : " << rcs.error_detail() << endl;
    return 4;
  }
  wtime = omp_get_wtime() - wtime;

  cout << "GPU finished in " << wtime * 1e3 << " ms" << endl;
  if (false) {
    for (size_t tid = 0; tid < task_size; tid++) {
      const std::array<float, 3> unconverted_color = tasks[tid];
      const uint16_t gpu_result_idx = rcs.result_idx()[tid];
      const float gpu_result_diff = rcs.result_diff()[tid];

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