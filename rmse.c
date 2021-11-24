#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "zfp.h"
#include "sz.h"
#include "util.h"
#include "compress.h"
#define Error_ratio 0.00001

#define Level 6
#define Refine_ratio 2


double RMSE(double *data_level, double *data_delta, int datasize){
	double sum = 0;
	int i;
	double rmse;
	for(i = 0; i < size; i++){
		sum += pow(data_level[i] - data_delta[i], 2);
	}
	rmse = sqrt(sum/(double)dsize);
	return rmse;
}


int main(int argc, char **argv)
{

	int i,j,cnt[Level],box_cnt[Level],read_cnt;
	struct datapoint** data;
	data=(struct datapoint**)malloc(Level*sizeof(* data)); 
	struct box** boxes;
	boxes=(struct box**)malloc(Level*sizeof(* boxes));

	int list[6]={0, 1, 4, 7, 10, 15}; 
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
		read_cnt=fread(&box_cnt[i],sizeof(int),1,fp);
		boxes[i]=(struct box *)malloc(box_cnt[i]*sizeof(struct box));
		read_cnt=fread(boxes[i],sizeof(struct box),box_cnt[i],fp);
		data[i]=(struct datapoint*)malloc(cnt[i]*sizeof(struct datapoint));
		read_cnt=fread(data[i],sizeof(struct datapoint),cnt[i],fp);
	}
	fclose(fp);



	mapping=(int**)malloc(Level*sizeof(* mapping));
	for(i=0;i<Level;i++)
		mapping[i]=(int*)malloc(cnt[i]*sizeof(int));

	box_mapping=(struct parent_box**)malloc(Level*sizeof(* box_mapping));
	for(i=0;i<Level;i++)
		box_mapping[i]=(struct parent_box*)malloc(box_cnt[i]*sizeof(struct parent_box));


	int datasize=0;
	for(i=0;i<Level;i++)
		datasize=datasize+cnt[i];
	double *data_delta=(double*)malloc(datasize*sizeof(double));
	double *data_level=(double*)malloc(datasize*sizeof(double));
	double *data_baseline=(double*)malloc(datasize*sizeof(double));
	int *   recipe_en_levelRe=(int*)malloc(datasize*sizeof(int));
	int *   recipe_en_baseline=(int*)malloc(datasize*sizeof(int));


  mapping_by_box(data,cnt,boxes,box_cnt);


	for(i=0;i<Level;i++)	
		free(data[i]);
	for(i=0;i<Level;i++)
		free(boxes[i]);


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
			boxes[i]=(struct box*)malloc(box_cnt[i]*sizeof(struct box));
			read_cnt=fread(boxes[i],sizeof(struct box),box_cnt[i],fp);
			data[i]=(struct datapoint*)malloc(cnt[i]*sizeof(struct datapoint));
			read_cnt=fread(data[i],sizeof(struct datapoint),cnt[i],fp);
		}
		fclose(fp);



		double max=-100000000;
		double min=100000000;

		int offset=0;
		for(i=0;i<Level;i++)
			for(j=0;j<cnt[i];j++)
			{
				data_level[offset]=data[i][j].val;
				data_delta[offset]=data[i][j].val;
				if( data_level[offset]>max)
					max= data_level[offset];
				if( data_level[offset]<min)
					min= data_level[offset];
				offset++;

			}

		double Errbound=Error_ratio*(max-min);

		if(Errbound>10000)
			Errbound=100;
    
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
		int c_size_zfp;
		int c_size_sz;
		int d_size_zfp;


		c_size_zfp=zfp1_compress(data_level,datasize,Errbound,"li");

		
		FILE *fp = fopen("li", "r");
		double *decompressed = (double*)malloc(datasize*sizeof(double));
		read_cnt = fread(decompressed, sizeof(double), datasize, fpzori);
		d_size_zfp = zfp1_decompress(decompressed, datasize, Errbound, "li");
		fclose(fp);
		double rmse_zfpori;
		rmse_zfpori = RMSE(data_level, decompressdata, datasize);
		printf("zfp decompress baseline RMSE :  %lf\n", rmse_zfpori);
    free(decompressed);
    free(compressed);
		

		compressed = SZ_compress(SZ_DOUBLE, data_level, &outSize, r5, r4, r3, r2 ,datasize);
		fp=fopen("temp","w");
                if (fp==NULL)
                {
                        printf("Can not open file\n");
                        return 0;

                }

		fwrite(compressed, 1, outSize, fp);
		fclose(fp);
    
    
		c_size_sz=outSize;	
		decompressed = SZ_decompress(SZ_DOUBLE, compressed, c_size_sz, r5, r4, r3, r2, datasize);
		double rmse_szori;
		rmse_szori = RMSE(data_level, decompressed, datasize);
		printf("sz decompress baseline RMSE :  %lf\n", rmse_szori);

		free(compressed);
		free(decompressed);
		

		int level_offset = datasize;
		for(i=Level-1;i>0;i--)
		{
			level_offset=level_offset-cnt[i];
			for(j=0;j<cnt[i];j++)
			{       
				int father_index=mapping[i][j];
				data_delta[ level_offset+j]=data[i][j].val-data[i-1][father_index].val;
			}

		}

		
		
		FILE *fp = fopen("li", "r");
		read_cnt = fread(decompressed, sizeof(double), datasize, fp);
		d_size_zfp = zfp1_decompress(decompressed, datasize, Errbound, "li");
		fclose(fp);

		
		int level_offset3=cnt[0], level_offset4=0;
		for(i=1;i<Level;i++){
			for(j=0;j<cnt[i];j++){
				int father_index3=mapping[i][j];
				decompressed[level_offset3] = decompressed[level_offset3] + decompressed[level_offset4+father_index3];
				level_offset3++;
			}
			level_offset4+=cnt[i];
		}
		
		
		double rmse_zfp;
		rmse_zfp = RMSE(data_level,decompressed,datasize);
		double rermse_zfp;
		rermse_zfp = rmse_zfp;
		printf("zfp decompress lamp RMSE : %lf\n", rermse_zfp);
		free(decompressed);
    free(compressed);


		compressed = SZ_compress(SZ_DOUBLE, data_delta, &outSize, r5, r4, r3, r2 ,datasize);
		fp=fopen("temp","w");
                if (fp==NULL)
                {
                        printf("Can not open file\n");
                        return 0;

                }
    fwrite(compressed, 1, outSize, fp);
    fclose(fp);

		c_size_sz=outSize;
		decompressed = SZ_decompress(SZ_DOUBLE, compressed, c_size_sz, r5, r4, r3, r2, datasize);
				
		level_offset3=cnt[0];
		level_offset4=0;
		for(i=1;i<Level;i++){
			for(j=0;j<cnt[i];j++){
				int father_index3=mapping[i][j];
				decompressed[level_offset3] = decompressed[level_offset3] + decompressed[level_offset4+father_index3];
				level_offset3++;
			}
			level_offset4+=cnt[i];
		}
		

		rmse_sz = RMSE(data_level, decompressed, datasize);
		rermse_sz = rmse_sz/Errbound;
		printf("SZ decompress lamp RMSE:  %lf\n", rermse_sz);

		free(compressed);
		free(decompressed);
	
	}

	printf("\n");

	for(i=0;i<Level;i++)	
		free(data[i]);
	for(i=0;i<Level;i++)
		free(boxes[i]);
	free(boxes);
	free(data);
	free(recipe_en_levelRe);
	free( recipe_en_baseline);
	free(data_level);
	free(data_delta);
	free(data_baseline);
	for(i=0;i<Level;i++)
		free(mapping[i]);
	free(mapping);
	for(i=0;i<Level;i++)
		free(box_mapping[i]);
	free(box_mapping);
	return 1;
}
