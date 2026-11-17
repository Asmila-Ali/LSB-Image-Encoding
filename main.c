#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo E1;

    //validate cla
    int res = check_operation_type(argv);
    if((res == e_encode) && argc >=4)
    {
            //printf("Encoding is selected\n");
            if(read_and_validate_encode_args(argv, &E1)==e_success)
            {
                //printf("Encode args is read and validated succesfully\n");
                if(open_encode_files(&E1)== e_success)
                {
                    //printf("Info: Files are opened succesfully\n");
                    if(do_encoding(&E1)==e_success)
                    {
                        printf("INFO: ## Encoding Done Succesfully ##\n");
                    }
                    else
                    {
                        printf("INFO: Failed to encode\n");
                        return e_failure;
                    }

                }
                else
                {
                    printf("INFO: Failed to open files\n");
                    return e_failure;
                }
            }
            else
            {
                printf("INFO: Failed to read and validate encode args\n");
                return e_failure;
            }
    }
    else if((res == e_decode)&& argc>=3)
    {
            //printf("Decoding is selected\n");
            DecodeInfo D1;
            if(read_and_validate_decode_args(argv, &D1)==e_success)
            {
                //printf("Read and validated decode arguments succesfully\n");
                if(open_decode_files(&D1)==e_success)
                {
                    //printf("Info: Files are opened succesfully\n");
                    if(do_decoding(&D1)==e_success)
                    {
                        printf("INFO: ## Decoding success ##\n");
                    }
                    else
                    {
                        printf("INFO: Failed to decode\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("INFO: Failed to open files\n");
                    return e_failure;
                }
            }
            else
            {
                printf("INFO: Failed to read and validate decode arguments\n");
                return e_failure;
            }
    }
    else
    {
        printf("Invalid option\n");
        printf("For encoding: ./a.out -e beautiful.bmp secret.txt [stegno.bmp]\n");
        printf("For decoding: ./a.out -d stegno.bmp default.txt\n");
        return e_unsupported;
    }

    return e_success;
}

