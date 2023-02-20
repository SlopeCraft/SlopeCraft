__kernel void match_color_test(
    __global const float*color_avaliable_ptr,
    const ushort color_avaliable_num_ptr,
    __global const uint*colors_unconverted,
    __global ushort* result_idx_dst,
    __global float* result_diff_dst
    ) {

        //const float p=color_avaliable_num_ptr[0];
        return;
    }

__kernel void match_color_RGB(
     __global const float* colors_avaliable_ptr,
    const ushort colorset_size,
    __global const uint*colors_unconverted,
    __global ushort* result_idx_dst,
    __global float* result_diff_dst) {
    
    const size_t global_idx=get_global_id(0);

    //const float3 unconverted=colors_unconverted[global_idx];
    float3 unconverted;
    {
        const uint ucv_u32=colors_unconverted[global_idx];
        unconverted[0]=ucv_u32&0x00FF0000;
        unconverted[1]=ucv_u32&0x0000FF00;
        unconverted[2]=ucv_u32&0x000000FF;
    }



    ushort result_idx=0;
    float result_diff=FLT_MAX/2;

    for(ushort idx=0; idx<colorset_size; idx++) {
        //const float3 color_ava=colors_avaliable_ptr[idx];
        const float3 color_ava=vload3(idx,colors_avaliable_ptr);

        float3 diff=color_ava-unconverted;
        diff*=diff;

        const float diff_sq=diff[0]+diff[1]+diff[2];
        // this branch may be optimized
        if(result_diff<diff_sq) {
            result_idx=idx;
            result_diff=diff_sq;
        }
    }

    result_idx_dst[global_idx]=result_idx;
    result_diff_dst[global_idx]=result_diff;
}
