#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zfp.h"
#include "sz.h"

#define Error_ratio 0.00001
#define Level 6
#define Refine_ratio 2
  
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
	int * recipe_en_levelRe=(int*)malloc(datasize*sizeof(int));
	int * recipe_en_baseline=(int*)malloc(datasize*sizeof(int));
  
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

	
		double total_ori_zfp;
		c_size_zfp=zfp1_compress(data_level,datasize,Errbound,"li");


		compressed = SZ_compress(SZ_DOUBLE, data_level, &outSize, r5, r4, r3, r2 ,datasize);
		fp=fopen("temp","w");
                if (fp==NULL)
                {
                        printf("Can not open file\n");
                        return 0;

                }

		fwrite(compressed, 1, outSize, fp);
		fclose(fp);
		free(compressed);
		c_size_sz=outSize;
		
		

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
    
		double size_zfp;
		size_zfp=zfp1_compress(data_delta,datasize,Errbound,"li");

		compressed = SZ_compress(SZ_DOUBLE, data_delta, &outSize, r5, r4, r3, r2 ,datasize);
		fp=fopen("temp","w");
                if (fp==NULL)
                {
                        printf("Can not open file\n");
                        return 0;

                }

                fwrite(compressed, 1, outSize, fp);
                fclose(fp);
		free(compressed);


	//MAC comparation
	//baseline
	int m_i;
	double m_sum1 = 0.0;
	double pan1;
	double orimac;
	for(m_i = 0; m_i < datasize-1; m_i++){
		pan1 = data_level[m_i + 1] - data_level[m_i];
		if(abs(pan1) > 1000.0)
			pan1 = 0.0;
		m_sum1 = m_sum1 + abs(pan1);
	}		
	orimac = m_sum1 / (double)datasize;
	printf("the baseline mac is : %lf\n", orimac);
	
	//LAMP
        double m_sum2 = 0.0;
	double pan2;
        double lampmac;
        for(m_i = 0; m_i < datasize-1; m_i++){
               pan2 = data_delta[m_i + 1] - data_delta[m_i];
		if(fabs(pan2) > 1000.0)
                        pan2 = 0.0;	       
	       	m_sum2 = m_sum2 + fabs(pan2);
        }
        lampmac = m_sum2 / (double)datasize;
        printf("the LAMP mac is : %lf\n", lampmac);
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
