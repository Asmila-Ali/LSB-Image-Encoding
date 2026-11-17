#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_encode_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s\n", encInfo->src_image_fname);
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s\n", encInfo->secret_fname);
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s\n",encInfo->stego_image_fname);
    }

    // No failure return e_success
    printf("INFO: Done\n");
    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //input file .bmp
    if(strstr(argv[2], ".bmp") != NULL)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }
    //input secret.txt
    if(strstr(argv[3], ".txt")!=NULL)
    {
        encInfo->secret_fname = argv[3];
    }
    else
    {
        return e_failure;
    }
    //output .bmp
    if(argv[4]!= NULL)
    {
        if(strstr(argv[4], ".bmp")!= NULL)
        {
            encInfo->stego_image_fname = argv[4];
        }
    }
    else
    {
        printf("INFO: Output file not mentioned. Creating steged_img.bmp as default\n");
        encInfo->stego_image_fname = "steged_img.bmp";
    }
    return e_success;
}

OperationType check_operation_type(char *argv[])
{
    if(argv[1]==NULL)
    {
        return e_unsupported;
    }
    else
    {
        if (strcmp(argv[1], "-e") == 0)
            return e_encode;
        else if (strcmp(argv[1], "-d") == 0)
            return e_decode;
        else
            return e_unsupported;
    }   
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0, SEEK_END);
    uint size=ftell(fptr);
    rewind(fptr);
    return size;
}

Status check_capacity(EncodeInfo *encInfo)
{
    printf("INFO: Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity > (112+ encInfo->size_secret_file* 8))
    {
        printf("INFO: Done. Found OK\n");
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO: Copying image header\n");
    char buffer[54];
    rewind(fptr_src_image);
    //read 54B from source image
    fread(buffer, 54, 1, fptr_src_image);
    //write to dest image
    fwrite(buffer, 54, 1, fptr_dest_image);
    printf("INFO: Done\n");
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8; i++)
    {
        //clear lsb bit and set it
        image_buffer[i]= (image_buffer[i] & 0xFE) | ((data>>i) &1); // 0xFE= 1111 1110
    }
    return e_success;
}

Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    for(int i=0; i<size; i++)
    {
        //read 8 byte of RGB Data 
        fread(encInfo->image_data, 8, 1,encInfo->fptr_src_image);
        //byte to lsb
        encode_byte_to_lsb(data[i], encInfo->image_data);
        //write encoded data to output
        fwrite(encInfo->image_data, 8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO: Encoding Magic String Signature\n");
    encode_data_to_image(magic_string, strlen(magic_string), encInfo);
    printf("INFO: Done\n");
    return e_success;
}

Status encode_size_to_lsb(int size, EncodeInfo *encInfo)
{
    char str[32];
    fread(str, 32, 1, encInfo->fptr_src_image);
    //encode size (int 32-bits) in 32 Bytes
    for(int i=0;i<32; i++)
    {
        str[i]= (str[i] & 0xFE) | ((size>>i) &1); 
    }
    fwrite(str,32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(size,encInfo);
    return e_success;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Encoding Secret file extension\n");
    encode_data_to_image(file_extn, strlen(file_extn), encInfo);
    printf("INFO: Done\n");
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding Secret file size\n");
    encode_size_to_lsb(file_size,encInfo);
    printf("INFO: Done\n");
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding Secret file data\n");
    char data[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);
    fgets(data, encInfo->size_secret_file, encInfo->fptr_secret);
    encode_data_to_image(data, encInfo->size_secret_file, encInfo);
    printf("INFO: Done\n");
    return e_success;
}

Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    printf("INFO: Copying left over data\n");
    int size = 54+ encInfo->image_capacity- ftell(encInfo->fptr_src_image);
    char buffer[size];
    fread(buffer, size, 1, encInfo->fptr_src_image);
    fwrite(buffer, size, 1, encInfo->fptr_stego_image);
    printf("INFO: Done\n");
    return e_success;

}

Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: ## Encoding Procedure Started ##\n");
    if(check_capacity(encInfo)==e_success)
    {
        //printf("Check capacity is success\n");
        if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
        {
            //printf("Info: Copied bmp header succesfully\n");
            if(encode_magic_string(MAGIC_STRING, encInfo)==e_success)
            {
                //printf("Encoded magic string succesfully\n");
                strcpy(encInfo->extn_secret_file , strstr(encInfo->secret_fname, ".")); //.txt
                //printf("%s\n", encInfo->extn_secret_file);
                if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo)==e_success)
                {
                    //printf("Info: Encoded secret file extension size succesfully\n");
                    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo)==e_success)
                    {
                        //printf("Encoded secret file extn succesfully\n");
                        if(encode_secret_file_size(encInfo->size_secret_file, encInfo)==e_success)
                        {
                            //printf("Encoded secret file size succesfully\n");
                            if(encode_secret_file_data(encInfo)==e_success)
                            {
                                //printf("Encoded secret file data succesfully\n");
                                if(copy_remaining_img_data(encInfo)==e_success)
                                {
                                    //printf("Remaining data copied to the destination file succesfully\n");
                                }
                                else
                                {
                                    printf("INFO: Failed to copy the remaing data to the destination file\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("INFO: Failed to encode secret file data\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("INFO: Failed to encode file size\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("INFO: Failed to encode secret file extn\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("INFO: Failed to encode secret file extension size\n");
                    return e_failure;
                }
            }
            else
            {
                printf("INFO: Failed to encode magic string\n");
                return e_failure;
            }
        }
        else
        {
            printf("INFO: Failed to copy bmp header\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO: Error: secret data cannot be fit in the given image\n");
        return e_failure;
    }

    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    return e_success;
}


