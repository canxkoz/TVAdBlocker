
#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

float helper(unsigned char* rgb_data, void* data);

void* init_data();

#ifdef __cplusplus
}
#endif