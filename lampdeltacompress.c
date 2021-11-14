#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define Level 5
#define Refine_ratio 2
#define SIZE_MAX1  2147483647
#define Error_ratio 0.00001
#include "compress.h"


struct datapoint{
	int a, b;
        double val;
	};

struct box{
        int x1, y1;
        int x2, y2;
        };

void boxconstinterpolate(struct datapoint **data, int cnt[Level], struct box **boxes, int box_cnt[Level], int low, int high)
{
	char b_filename[50];
        sprintf(b_filename,"Green_value1_delta_%d_%d_new",low,high);
        FILE *b_fp=fopen(b_filename,"w");
	int lbox_sizearr[box_cnt[low]];
       	int hbox_sizearr[box_cnt[high]];
	int h_i, l_i;
	for(l_i = 0; l_i < box_cnt[low]; l_i++){
                lbox_sizearr[l_i] = (boxes[low][l_i].x2 - boxes[low][l_i].x1 + 1) * (boxes[low][l_i].y2 - boxes[low][l_i].y1 + 1);
        }
        for(h_i = 0; h_i < box_cnt[high]; h_i++){
                hbox_sizearr[h_i] = (boxes[high][h_i].x2 - boxes[high][h_i].x1 + 1) * (boxes[high][h_i].y2 - boxes[high][h_i].y1 + 1);
        }
	for(h_i = 0; h_i < box_cnt[high]; h_i++){
		for(l_i = 0; l_i < box_cnt[low]; l_i++){
			if((boxes[high][h_i].x1/2 == boxes[low][l_i].x1) && (boxes[high][h_i].y1/2 == boxes[low][l_i].y1)){
				int hbox_size = hbox_sizearr[h_i];
				int lbox_size = lbox_sizearr[l_i];
				int h_x1 = boxes[high][h_i].x1;
				int h_y1 = boxes[high][h_i].y1;
				int l_x1 = boxes[low][l_i].x1;
				int l_y1 = boxes[low][l_i].y1;
				int h_data, l_data;
				int h_tem, l_tem;
				double delta;
			
				if(data[high][h_data].a == h_x1 && data[high][h_data].b == h_y1 && data[low][l_data].a == l_x1 && data[low][l_data].b == l_y1){
					for(h_tem = 0; h_tem < hbox_size; h_tem++){
						for(l_tem = 0; l_tem < lbox_size; l_tem++){
							if((data[high][h_data + h_tem].a/2 == data[low][l_data + l_tem].a) &&(data[high][h_data + h_tem].b/2 == data[low][l_data + l_tem].b)){
								delta = data[high][h_data + h_tem].val - data[low][l_data + l_tem].val;
								{
									fwrite(&delta, sizeof(double), 1, b_fp);
								}
								break;
							}
						}
					}
				}
			}
		}
	}
}

/*
void constinterpolate(struct datapoint **data, int cnt[Level],int low, int high)
{
        int c_i,c_j; double delta;
        char c_filename[50];
        sprintf(c_filename,"Green_value2_delta_%d_%d",low,high);
        FILE *c_fp=fopen(c_filename,"w");
        for(c_j=0;c_j<cnt[high];c_j++)
                for(c_i=0;c_i<cnt[low];c_i++)
                {
                        if(data[low][c_i].a==data[high][c_j].a/2&&data[low][c_i].b==data[high][c_j].b/2)
                        {
                                delta=data[high][c_j].val-data[low][c_i].val;
                                {
					fwrite(&delta,sizeof(double),1,c_fp);
                                }
                                break;
                        }
                }
        fclose(c_fp);
}
*/

void leveldata(struct datapoint **data, int cnt[Level])
{
        int l_i,l_j;
        for(l_i=0;l_i<Level;l_i++)
        {
                char l_filename[50];
                sprintf(l_filename,"Green_value1_Level_%d.dat",l_i);
                FILE *l_fp=fopen(l_filename,"w");
                for(l_j=0;l_j<cnt[l_i];l_j++)
                {
                        fwrite(&data[l_i][l_j].val,sizeof(double),1,l_fp);
                }

                fclose(l_fp);
        }
}

int main(int argc, char** argv)
{
	int begintime, endtime;
	int i, j, cnt[Level], box_cnt[Level], read_cnt;
	struct datapoint** data;
	data = malloc(Level * sizeof(*data));

	struct box** boxes;
	boxes = malloc(Level * sizeof(*boxes));

	char filename[50];
	int selectlist[6] = {1, 2, 6, 12, 13, 15};		
	sprintf(filename, "Green_datapoint_%d", selectlist[0]);
	
	FILE* fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("Can not open file\n");
		return 0;

	}


	int sum = 0;
	for (i = 0; i < Level; i++)
	{
		read_cnt = fread(&cnt[i], sizeof(int), 1, fp);
		read_cnt = fread(&box_cnt[i], sizeof(int), 1, fp);
		boxes[i] = malloc(box_cnt[i] * sizeof(struct box));
		read_cnt = fread(boxes[i], sizeof(struct box), box_cnt[i], fp);
		data[i] = malloc(cnt[i] * sizeof(struct datapoint));
		read_cnt = fread(data[i], sizeof(struct datapoint), cnt[i], fp);
		sum += cnt[i];
	}
	fclose(fp);
	
	//printf("%d %d\n", data[1][0].a, data[1][0].b);
	//printf("%lf\n", data[1][0].val);
	//printf("%d %d\n", boxes[1][0].x1, boxes[1][0].y1);
	//printf("%d\n", box_cnt[1]);
	//printf("%d\n", cnt[1]);

	int template = 0;
	double* data_level_onebyone = malloc(sum * sizeof(double));
	for(i = 0; i < Level; i++){
		for(j = 0; j < cnt[i]; j++){
			data_level_onebyone[template] = data[i][j].val;
			template++;
		}
	}
	
	double max = data_level_onebyone[0];
	double min = data_level_onebyone[0];
	//begintime = clock();
	for(i = 1; i < sum; i++){
		if(data_level_onebyone[i] > max){
			max = data_level_onebyone[i];
		}
		if(data_level_onebyone[i] < min){
			min = data_level_onebyone[i];
		}
	}
	//endtime = clock();
	//printf("Running time is %d ms\n", endtime - begintime);
/*
	int offset=0;
	double max=-100000000;
	double min=100000000;
	double *data_level = malloc(sum * sizeof(double));
	begintime = clock();
	for(i=0;i<Level;i++){
        	for(j=0;j<cnt[i];j++)
                	{
                        	data_level[offset]=data[i][j].val;
                                if( data_level[offset]>max)
                                        max= data_level[offset];
                                if( data_level[offset]<min)
                                        min= data_level[offset];
                                offset++;

                        }
	}
	endtime = clock();
	printf("Running time is %d ms\n", endtime - begintime);
*/
        double Errbound=Error_ratio*(max-min);
        if(Errbound>10000)
        	Errbound=100;
		
	printf("Errbound : %lf\n", Errbound);

	double fullsize=sum*sizeof(double);
        double compressedsize=0;

	printf("============================level_by_level ZFP=====================================\n");
	//int nx = datasize / 4;
	//int ny = 4;
	compressedsize=zfp2_compress(data_level_onebyone,sum,0,Errbound);
	printf("%lf\n",fullsize/compressedsize);


	printf("============================level_by_level SZ======================================\n");
	
	char *confFile = "/home/liyida/Compressor/SZ/example/sz.config";
	size_t outSize;
	int r4 = 0, r3 = 0, r2 = 0, r5 = 0;
	SZ_Init(confFile);
	confparams_cpr->absErrBound =Errbound; 
	unsigned char *compressed;
	compressed = SZ_compress(SZ_DOUBLE, data_level_onebyone, &outSize, r5, r4, r3, 0, sum);
	free(compressed);
	compressedsize=outSize;
	printf("%lf\n",fullsize/compressedsize);
	/*
	int b_i, b_j;
	int b_min = 0;
	int b_max = 0;
	for(b_i = 0; b_i < Level; b_i++){
		printf("the level is %d\n", b_i+1);
		for(b_j = 0; b_j < box_cnt[b_i]; b_j++){
			//printf("box = %d  %d  %d  %d\n", boxes[b_i][b_j].x1, boxes[b_i][b_j].y1, boxes[b_i][b_j].x2, boxes[b_i][b_j].y2);
			if(boxes[b_i][b_j].y1 < b_min)
				b_min = boxes[b_i][b_j].y1;
			if(boxes[b_i][b_j].y2 > b_max)
				b_max = boxes[b_i][b_j].y2;
		}
		printf("the min y1 = %d  the max y2 = %d\n", b_min, b_max);
	}	
	*/
	
	begintime = clock();
	for(i = 0; i < Level - 1; i++){
		boxconstinterpolate(data, cnt, boxes, box_cnt, i, i + 1);
	}
	endtime = clock();
	printf("Running time is %d ms\n", endtime - begintime);
	
	//const interpolate
	/*
	begintime = clock();
	for(i=0;i<Level-1;i++){
                constinterpolate(data,cnt,i,i+1);
	}	
	endtime = clock();
	printf("Running time is %d ms\n", endtime - begintime);
	*/
	leveldata(data,cnt);	

	int d_d;
	double *full_data_array = malloc(sum*sizeof(double));
	double d_fullread_cnt;
	for(d_d=0; d_d < cnt[0]; d_d++){
		full_data_array[d_d] = data[0][d_d].val;
	}
	
        int d_i;
        double d_read_cnt1;
        FILE *fp1=fopen("Green_value1_delta_0_1_new","r");
        for(d_i=cnt[0];d_i<cnt[0] + cnt[1];d_i++){
                d_read_cnt1 = fread(&full_data_array[d_i],sizeof(double),1,fp1);
        }
        fclose(fp1);

        int d_j;
        double d_read_cnt2;
        FILE *fp2=fopen("Green_value1_delta_1_2_new","r");
        for(d_j=cnt[0]+cnt[1];d_j<cnt[0]+cnt[1]+cnt[2];d_j++){
                d_read_cnt2 = fread(&full_data_array[d_j],sizeof(double),1,fp2);
        }
        fclose(fp2);

	int d_k;
        double d_read_cnt3;
        FILE *fp3=fopen("Green_value1_delta_2_3_new","r");
        for(d_k=cnt[0]+cnt[1]+cnt[2]; d_k<cnt[0]+cnt[1]+cnt[2]+cnt[3]; d_k++){
                d_read_cnt3 = fread(&full_data_array[d_k],sizeof(double),1,fp3);
        }
        fclose(fp3);

        int d_z;
        double d_read_cnt4;
        FILE *fp4=fopen("Green_value1_delta_3_4_new","r");
        for(d_z=sum-cnt[4]; d_z<sum; d_z++){
                d_read_cnt4 = fread(&full_data_array[d_z],sizeof(double),1,fp4);
        }
        fclose(fp4);
	
	printf("============================Constant_interpolate ZFP=====================================\n");
        compressedsize=zfp2_compress(full_data_array,sum,0,Errbound);
        printf("%lf\n",fullsize/compressedsize);

        printf("============================Constant_interpolate SZ======================================\n");
        char *d_confFile = "/home/liyida/Compressor/SZ/example/sz.config";
        size_t d_outSize;
        SZ_Init(d_confFile);
        confparams_cpr->absErrBound =Errbound;
        unsigned char *d_compressed;
        d_compressed = SZ_compress(SZ_DOUBLE, full_data_array, &d_outSize, 0, 0, 0, 0, sum);
        free(d_compressed);
        compressedsize=d_outSize;
        printf("%lf\n",fullsize/compressedsize);
	


	for (i = 0; i < Level; i++)
		free(data[i]);
	for (i = 0; i < Level; i++)
		free(boxes[i]);


	free(boxes);
	free(data);
	//free(data_level);
	free(data_level_onebyone);
	free(full_data_array);
	return 1;
}
