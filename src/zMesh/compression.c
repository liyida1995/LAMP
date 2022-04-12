#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define Level 6
#define Refine_ratio 2
#define SIZE_MAX1  2147483647
#define Error_ratio 0.00001

#include "util.h"
#include "compress.h"


int main(int argc, char **argv)
{
	int i,j,cnt[Level],box_cnt[Level],read_cnt;
	struct datapoint** data;
	data=malloc(Level*sizeof(* data)); 
	struct box** boxes;
	boxes=malloc(Level*sizeof(* boxes));

	int list[6]={0,2,4,7,10,15}; 
	char filename[50];
	sprintf(filename,"datapoint_%d",list[0]);


	FILE *fp=fopen(filename,"r");
	if (fp==NULL)
	{
		printf("Can not open file\n");
		return 0;

	}




	for(i=0;i<Level;i++)
	{
		read_cnt=fread(&cnt[i],sizeof(int),1,fp);
		//	printf("data count= %d\n", cnt[i]);
		read_cnt=fread(&box_cnt[i],sizeof(int),1,fp);
		//	printf("box_cnt= %d\n", box_cnt[i]);
		boxes[i]=malloc(box_cnt[i]*sizeof(struct box));
		read_cnt=fread(boxes[i],sizeof(struct box),box_cnt[i],fp);
		data[i]=malloc(cnt[i]*sizeof(struct datapoint));
		read_cnt=fread(data[i],sizeof(struct datapoint),cnt[i],fp);
		//for(j=0;j<cnt[i];j++)
		//	printf("%d %d %lf\n", data[i][j].a,data[i][j].b,data[i][j].val);
	}
	fclose(fp);

	mapping=malloc(Level*sizeof(* mapping));
	for(i=0;i<Level;i++)
		mapping[i]=malloc(cnt[i]*sizeof(int));

	box_mapping=malloc(Level*sizeof(* box_mapping));
	for(i=0;i<Level;i++)
		box_mapping[i]=malloc(box_cnt[i]*sizeof(struct parent_box));




	int datasize=0;
	for(i=0;i<Level;i++)
		datasize=datasize+cnt[i];
	double *data_levelRe=malloc(datasize*sizeof(double));
	double *data_level=malloc(datasize*sizeof(double));
	double *data_baseline=malloc(datasize*sizeof(double));
	int *   recipe_en_levelRe=malloc(datasize*sizeof(int));
	int *   recipe_en_baseline=malloc(datasize*sizeof(int));



	get_zorder_encode_recipe(recipe_en_baseline,cnt,boxes,box_cnt);
	get_zorder_zmesh_encode_recipe(data, recipe_en_levelRe,cnt,boxes,box_cnt);





	for(i=0;i<Level;i++)	
		free(data[i]);
	for(i=0;i<Level;i++)
		free(boxes[i]);
	for(i=0;i<Level;i++)
		free(mapping[i]);
	free(mapping);
	for(i=0;i<Level;i++)
		free(box_mapping[i]);
	free(box_mapping);
	double  t_compress_levelRe_zfp=0;
	double  t_compress_levelRe_sz=0;
	double  fullsize=datasize*sizeof(double);
	double compressedsize=0;
	double compressedsize_baseline=0;
	double compressedsize_levelRe=0;
	for(int col=0;col<6;col++){
		sprintf(filename,"datapoint_%d",list[col]);

		fp=fopen(filename,"r");
		if (fp==NULL)
		{
			printf("Can not open file\n");
			return 0;

		}




		for(i=0;i<Level;i++)
		{
			read_cnt=fread(&cnt[i],sizeof(int),1,fp);
			read_cnt=fread(&box_cnt[i],sizeof(int),1,fp);
			boxes[i]=malloc(box_cnt[i]*sizeof(struct box));
			read_cnt=fread(boxes[i],sizeof(struct box),box_cnt[i],fp);
			data[i]=malloc(cnt[i]*sizeof(struct datapoint));
			read_cnt=fread(data[i],sizeof(struct datapoint),cnt[i],fp);
			//for(j=0;j<cnt[i];j++)
			//	printf("%d %d %lf\n", data[i][j].a,data[i][j].b,data[i][j].val);
		}
		fclose(fp);

		int offset=0;
		double max=-100000000;
		double min=100000000;
		for(i=0;i<Level;i++)
			for(j=0;j<cnt[i];j++)
			{
				data_level[offset]=data[i][j].val;
				if( data_level[offset]>max)
					max= data_level[offset];
				if( data_level[offset]<min)
					min= data_level[offset];
				offset++;

			}
		double Errbound=Error_ratio*(max-min);
		//	double Errbound=Error_ratio*max;
		if(Errbound>10000)
			Errbound=100;

		for(int i1=0;i1<datasize;i1++)
		{
			data_levelRe[i1]=data_level[recipe_en_levelRe[i1]]; 
			data_baseline[i1]=data_level[recipe_en_baseline[i1]]; 
		}  
		//		printf("EB:%lf\n",Errbound);
		char compressed_name[50];
		sprintf(compressed_name,"LevelRe_%d.zfp",list[col]);  
		
		
		compressedsize=zfp1_compress(data_levelRe,datasize,Errbound);
		//printf("%lf\n", fullsize/compressedsize);	

		sz_params sz;
    		memset(&sz, 0, sizeof(sz_params));
    		sz.sol_ID = SZ;
    		sz.sampleDistance = 100;
   	 	sz.quantization_intervals = 0;
    		sz.max_quant_intervals = 65536;
    		sz.predThreshold = 0.98;
    		sz.szMode = SZ_BEST_COMPRESSION;
    		sz.losslessCompressor = ZSTD_COMPRESSOR;
    		sz.gzipMode = 1;
    		sz.errorBoundMode = ABS;
    		sz.absErrBound = 1;
    		SZ_Init_Params(&sz);
    		size_t outSize;
    		confparams_cpr->absErrBound =Errbound;

		int r4 = 0, r3 = 0, r2 = 0, r5 = 0;
		unsigned char *compressed;
		double* decompressed;
		
		compressed = SZ_compress(SZ_DOUBLE, data_levelRe, &outSize, r5, r4, r3, r2 ,datasize);
		compressedsize = outSize;
		
		printf("%lf\n", fullsize/compressedsize);
		
		free(compressed);

		for(i=0;i<Level;i++)	
			free(data[i]);
		for(i=0;i<Level;i++)
			free(boxes[i]);
	}
	free(boxes);
	free(data);
	free(recipe_en_levelRe);
	free( recipe_en_baseline);
	free(data_level);
	free(data_levelRe);
	free(data_baseline);
	return 1;
}
