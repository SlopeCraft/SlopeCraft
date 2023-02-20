__kernel void match_color_RGB(
    __global const float3* colors_avaliable_ptr,
    global const ushort* color_avaliable_num_ptr,
    __global const uint*colors_unconverted,
    __global ushort* result_idx_dst,
    __global float* result_diff_dst) {
    
    const size_t global_idx=get_global_id(0);

    const float3 unconverted=colors_unconverted[global_idx];
    const ushort color_avaliable_num=*color_avaliable_num_ptr;

    ushort result_idx=0;
    float result_diff=FLT_MAX/2;

    for(ushort idx=0; idx<color_avaliable_num; idx++) {
        const float3 color_ava=colors_avaliable_ptr[idx];

        float3 diff=color_ava-unconverted;
        diff=diff*diff;

        const float diff_sq=diff[0]+diff[1]+diff[2];
        // this branch may be optimized
        if(result_diff>diff_sq) {
            result_idx=idx;
            result_diff=diff_sq;
        }
    }

    result_idx_dst[global_idx]=result_idx;
    result_diff_dst[global_idx]=result_diff;
}
