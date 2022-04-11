#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sz.h"
#include "zfp.h"
#define Refine_ratio 2
#define ratioEB 0.00001

#define Level 6





double * fp_glob;
int procDataCnt;
int ** mapping;

struct parent_box{
	int num;
	int p_list[20];
	int offset[20];
};
struct parent_box **box_mapping;

struct datapoint{
	int a,b;
	double val;
};

struct box{
	int x1,y1;
	int x2,y2;
};

struct node{
	int x,y;
	double val;
	int flag;
	struct node *fchild;
	struct node *next;
	int index;
	int nproc;
};


struct node_b{
	int x,y;
	double val;
	int nproc;
};

void PrintTree(struct node* p,  int flag)
{
	if (p==NULL) 
		return;
	if(flag==1){
		fp_glob[0]= p->val;
		procDataCnt++;
	}
	else
	{
		p->val=fp_glob[0];
	}
	fp_glob++;
	p->flag=1;
	if (p->fchild!=NULL) 
	{ PrintTree(p->fchild,flag); }
	if (p->next!=NULL)
	{ PrintTree(p->next,flag); } 
}


int find_father(struct datapoint *data, int size,int a, int b)
{

	int i;
	for(i=0;i<size;i++)
		if(data[i].a==a&&data[i].b==b)
			return i;
	printf("do not find (%d,%d)\n",a,b);
	return -1;
}


int compare_box(struct box  b1, struct box  b2)
{
	if(b1.x1/Refine_ratio<b2.x1)
		return 0;

	if(b1.y1/Refine_ratio<b2.y1)
		return 0;

	if(b1.x2/Refine_ratio>b2.x2)
		return 0;

	if(b1.y2/Refine_ratio>b2.y2)
		return 0;

	return 1;

}

int child_cover_parent(struct box  b1, struct box  b2){
	if(b1.x1/Refine_ratio>b2.x1)
		return 0;

	if(b1.y1/Refine_ratio>b2.y1)
		return 0;

	if(b1.x2/Refine_ratio<b2.x2)
		return 0;

	if(b1.y2/Refine_ratio<b2.y2)
		return 0;

	return 1;

}

int find_leftup_box(struct box  b1, struct box  b2){
	if(b1.x1/Refine_ratio<b2.x1)
		return 0;

	if(b1.y1/Refine_ratio<b2.y1)
		return 0;

	if(b1.x2/Refine_ratio<b2.x2&&b1.y2/Refine_ratio<b2.y2)
		return 0;

	if(b1.x1/Refine_ratio>b2.x2)
		return 0;
	if(b1.y1/Refine_ratio>b2.y2)
		return 0;

	return 1;

}



int isoverlap_box(struct box  b1, struct box  b2)
{
	if(b1.x2/Refine_ratio<b2.x1||b1.y2/Refine_ratio<b2.y1)
		return 0;
	if(b1.x1/Refine_ratio>b2.x2||b1.y1/Refine_ratio>b2.y2)
		return 0;

	return 1;


}

struct box* find_overlap(struct box  b1, struct box  b2)
{
	struct box *overlap=malloc(sizeof(struct box));

	if(b1.x1/Refine_ratio>=b2.x1)
		overlap->x1=b1.x1;
	else
		overlap->x1=b2.x1*Refine_ratio;

	if(b1.x2/Refine_ratio<=b2.x2)
		overlap->x2=b1.x2;
	else
		overlap->x2=b2.x2*Refine_ratio+1;

	if(b1.y1/Refine_ratio>=b2.y1)
		overlap->y1=b1.y1;
	else
		overlap->y1=b2.y1*Refine_ratio;

	if(b1.y2/Refine_ratio<=b2.y2)
		overlap->y2=b1.y2;
	else
		overlap->y2=b2.y2*Refine_ratio+1;


	return overlap;
}


void init_box_mapping(struct box **boxes,int box_cnt[Level])
{
	int i,j,k,l;
	for(i=Level-1;i>0;i--)
	{
		for(j=0;j<box_cnt[i];j++)
		{
			int offset=0;
			int cursor=0;
			/*Child box is overlaped with only one parent box*/ 
			for( k=0;k<box_cnt[i-1];k++)
				if(compare_box(boxes[i][j],boxes[i-1][cursor]))
				{
					box_mapping[i][j].num=1;
					box_mapping[i][j].p_list[0]=cursor;
					box_mapping[i][j].offset[0]=offset;

					break;

				}
				else{
					offset=offset+(boxes[i-1][cursor].x2-boxes[i-1][cursor].x1+1)*(boxes[i-1][cursor].y2-boxes[i-1][cursor].y1+1);
					cursor=(cursor+1)%box_cnt[i-1];
				}

			/*Child box is overlaped with more than one parent box*/ 
			if(k>=box_cnt[i-1])
			{
				offset=0;
				box_mapping[i][j].num=0;
				for( l=0;l<box_cnt[i-1];l++){

					if(isoverlap_box(boxes[i][j],boxes[i-1][l]))
					{
						box_mapping[i][j].p_list[ box_mapping[i][j].num]=l;
						box_mapping[i][j].offset[ box_mapping[i][j].num]=offset;
						box_mapping[i][j].num++;
					}

					offset=offset+(boxes[i-1][l].x2-boxes[i-1][l].x1+1)*(boxes[i-1][l].y2-boxes[i-1][l].y1+1);



				}

			}



		}//j
	}//i


}


void mapping_by_box(struct datapoint **data, int cnt [Level],struct box **boxes,int box_cnt[Level])
{
	int i,j,k;
	int offset;
	int off1;

	init_box_mapping(boxes,box_cnt);

	for(i=Level-1;i>0;i--)
	{
		off1=0;
		for(j=0;j<box_cnt[i];j++)
		{

			int width=boxes[i][j].x2-boxes[i][j].x1+1;
			int height=boxes[i][j].y2-boxes[i][j].y1+1;
			int box_size=width*height; 


			if(box_mapping[i][j].num==1) 
			{

				offset=box_mapping[i][j].offset[0];
				int p_index=box_mapping[i][j].p_list[0];
				int width_f=boxes[i-1][p_index].x2-boxes[i-1][p_index].x1+1;

				for (int i1=0;i1<height;i1++)
				{


					int off_y=(boxes[i][j].y1+i1)/Refine_ratio-boxes[i-1][p_index].y1;	

					for (int j1=0;j1<width;j1++)
					{

						int off_x=(boxes[i][j].x1+j1)/Refine_ratio-boxes[i-1][p_index].x1;

						mapping[i][off1+i1*width+j1]=offset+off_x+off_y*width_f;


					}
				}
			}

			else
			{ 

				int p_num=box_mapping[i][j].num;
				for(k =0;k<p_num;k++)
				{
					int  p_index=box_mapping[i][j].p_list[k];
					struct box *overlap=find_overlap(boxes[i][j],boxes[i-1][p_index]); 
					offset=box_mapping[i][j].offset[k];
					int width_f=boxes[i-1][p_index].x2-boxes[i-1][p_index].x1+1;
					for (int i1=overlap->y1;i1<=overlap->y2;i1++)
					{


						int off_y=i1/Refine_ratio-boxes[i-1][p_index].y1;
						for (int j1=overlap->x1;j1<=overlap->x2;j1++)
						{

							int off_x=j1/Refine_ratio-boxes[i-1][p_index].x1;

							mapping[i][off1+(i1-boxes[i][j].y1)*width+j1-boxes[i][j].x1]=offset+off_x+off_y*width_f;


						}
					}


					free(overlap);
				}







			}//Child box are overlaped with more than one parent box.



			off1=off1+box_size;

		}//j for




	}



}

int zfp1D_compress (double * array, int nx, double tolerance
	       )


{
	zfp_type type;     /* array scalar type */
	zfp_field* field;  /* array meta data */
	zfp_stream* zfp;   /* compressed stream */
	void* buffer;      /* storage for compressed stream */
	size_t bufsize;    /* byte size of compressed buffer */
	bitstream * stream; /* bit stream to write to or read from */
	size_t zfpsize;    /* byte size of compressed stream */

	/* allocate meta data for the 3D array a[nz][ny][nx] */
	type = zfp_type_double;
	field = zfp_field_1d (array, type, nx);

	/* allocate meta data for a compressed stream */
	zfp = zfp_stream_open (NULL);

	/* set compression mode and parameters via one of three functions */
	/*  zfp_stream_set_rate(zfp, rate, type, 3, 0); */
	/* A_precision = zfp_stream_set_precision(zfp, precision, type);*/
	zfp_stream_set_accuracy (zfp, tolerance);


	/* allocate buffer for compressed data */
	bufsize = zfp_stream_maximum_size (zfp, field);
	buffer = malloc (bufsize);
	assert (buffer);
	/* associate bit stream with allocated buffer */
	stream = stream_open (buffer, bufsize);
	zfp_stream_set_bit_stream (zfp, stream);
	zfp_stream_rewind (zfp);

	/* compress array and output compressed stream */
	//zfpsize = zfp_compress (zfp, field);
	zfpsize = zfp_compress(zfp, field);
	if (!zfpsize) {
		fprintf(stderr, "compression failed\n");
	}
	assert (zfpsize);
	zfp_field_free (field);
	zfp_stream_close (zfp);
	stream_close (stream);

	free( buffer);

	return zfpsize;
}






void leveldata_box(struct datapoint **data, int cnt [Level],struct box **boxes,int box_cnt[Level],int col)
{
	clock_t start_t, end_t; double total_t;

	start_t = clock();
	int i,j;
	char filename[50];
	sprintf(filename,"Level_box_%d.dat",col);
	FILE *fp=fopen(filename,"w");

	for(i=0;i<Level;i++)
	{
		for(j=0;j<cnt[i];j++)
		{
			fwrite(&data[i][j].val,sizeof(double),1,fp);
		}

	}
	fclose(fp);

}




void leveldata_box_level(struct datapoint **data, int cnt [Level],struct box **boxes,int box_cnt[Level],int col)
{
	clock_t start_t, end_t, total_t;

	start_t = clock();
	int i,j;
	char filename[50];
	sprintf(filename,"Level_box_levelreordering_%d.dat",col);
	FILE *fp=fopen(filename,"w");

	mapping_by_box(data,cnt,boxes,box_cnt);
	struct node** trees;
	trees=malloc(Level*sizeof(* trees));
	for(i=0;i<Level;i++)
		trees[i]=malloc(cnt[i]*sizeof(struct node));
	for(i=0;i<Level;i++)

	{

		for(j=0;j<cnt[i];j++)
		{


			trees[i][j].x=data[i][j].a;
			trees[i][j].y=data[i][j].b;
			trees[i][j].val=data[i][j].val;
			trees[i][j].flag=0;
		}

	}


	for(i=Level-1;i>0;i--)
	{
		for(j=0;j<cnt[i];j++)
		{       

			int father_index=mapping[i][j];
			struct	node *p=trees[i-1][father_index].fchild;
			if (p==NULL)
				trees[i-1][father_index].fchild=&trees[i][j];
			else
			{
				while(p->next!=NULL)
					p=p->next;
				p->next=&trees[i][j];
			}




		}


	}

	int datasize=0;
	for(i=0;i<Level;i++)
		datasize=datasize+cnt[i];
	double *dataArray=malloc(datasize*sizeof(double));

	fp_glob=dataArray;
	for(i=0;i<1;i++)
		for(j=0;j<cnt[i];j++)
		{
			struct  node *p=&trees[i][j];
			PrintTree(p,1);

		}
	fclose(fp);
	free(dataArray);
	for(i=0;i<Level;i++)
		free(trees[i]);
	free(trees);
}






void leveldata_box_zordering(struct datapoint **data, int cnt [Level],struct box **boxes,int box_cnt[Level],int **processors,int col,int nproc)
{
	int i,j,k;

	struct node_b** trees;
	trees=malloc(Level*sizeof(* trees));
	for(i=0;i<Level;i++)
		trees[i]=malloc(cnt[i]*sizeof(struct node_b));
	int datasize=0;
	for(i=0;i<Level;i++)
		datasize=datasize+cnt[i];
	double *dataArray=malloc(datasize*sizeof(double));
	int offset=0;

	for(i=0;i<Level;i++)
	{ 
		offset=0;
		for(j=0;j<box_cnt[i];j++)
		{
			int a=boxes[i][j].y2-boxes[i][j].y1;
			int b=boxes[i][j].x2-boxes[i][j].x1;
			int box_size=(a+1)*(b+1);
			for(k=0;k<box_size;k++)
			{
				trees[i][offset+k].x=data[i][offset+k].a;
				trees[i][offset+k].y=data[i][offset+k].b;
                                trees[i][offset+k].val=data[i][offset+k].val;
				trees[i][offset+k].nproc=processors[i][j];
			}

			offset+=box_size;
		}

	}

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
	unsigned char *bytes;
	double max=-100000000;
	double min=100000000;

	int offset1=0;
	for(i=0;i<Level;i++)
		for(j=0;j<cnt[i];j++)
		{
			if(trees[i][j].val>max)
				max=trees[i][j].val;
			if(trees[i][j].val<min)
				min=trees[i][j].val;

			dataArray[offset1]=trees[i][j].val;
			offset1++;
		}
	
	double EB=(max-min)*ratioEB;
	if(EB>10000)
		EB=100;
	
	sz.absErrBound = EB;

	SZ_Init_Params(&sz);
	size_t outSize;
	size_t c_size=0;
	size_t c_size_zfp=0;



	c_size=0;
	c_size_zfp=0;
	for(int proc=0;proc<nproc;proc++){
		int offset1=0;
		for(i=0;i<Level;i++)
			for(j=0;j<cnt[i];j++)
			{
				if(trees[i][j].nproc==proc){
					dataArray[offset1]=trees[i][j].val;
					offset1++;
				}

			}
		if(offset1){
			bytes    = SZ_compress(SZ_DOUBLE, dataArray, &outSize, 0, 0, 0, 0,offset1);
			c_size+=outSize;
			free(bytes);
			c_size_zfp+=zfp1D_compress (dataArray, offset1, EB );
		}

	}
	
	printf("%lf\n",(double)datasize*sizeof(double)/c_size_zfp);
	printf("%lf\n",(double)datasize*sizeof(double)/c_size);
	for(i=0;i<Level;i++)
		free(trees[i]);
	free(trees);




	free(dataArray);
}



void leveldata_box_zordering_level(struct datapoint **data, int cnt [Level],struct box **boxes,int box_cnt[Level],int** processors,int col,int nproc)
{
	char filename[50];
	sprintf(filename,"Level_box_zordering_levelreordering_%d.dat",col);
	FILE *fp=fopen(filename,"w");

	clock_t start_t, end_t; double total_t;
	clock_t start_t1;
	start_t = clock();
	start_t1=start_t;
	int i,j,k;
	struct node** trees;
	trees=malloc(Level*sizeof(* trees));
	for(i=0;i<Level;i++)
		trees[i]=malloc(cnt[i]*sizeof(struct node));
	int datasize=0;
	for(i=0;i<Level;i++)
		datasize=datasize+cnt[i];
	double *data_delta=(double*)malloc(datasize*sizeof(double));

	mapping_by_box(data,cnt,boxes,box_cnt);
	int offset=0;

	for(i=0;i<Level;i++)
		for(j=0;j<cnt[i];j++)
		{

			trees[i][j].flag=0;

		}


	int* recipe_de;

	for(i=0;i<Level;i++)
	{ 
		offset=0;

		if(i<Level-1)
			recipe_de=malloc(cnt[i]*sizeof(int));
		for(j=0;j<box_cnt[i];j++)
		{
			int a=boxes[i][j].y2-boxes[i][j].y1;
			int b=boxes[i][j].x2-boxes[i][j].x1;
			int box_size=(a+1)*(b+1);
			for(k=0;k<box_size;k++)
			{
				trees[i][offset+k].x=data[i][offset+k].a;
                                trees[i][offset+k].y=data[i][offset+k].b;
                                trees[i][offset+k].val=data[i][offset+k].val;
                                trees[i][offset+k].index=offset+k;
				trees[i][offset+k].flag=0;
				trees[i][offset+k].fchild=NULL;
				trees[i][offset+k].next=NULL;
				trees[i][offset+k].nproc=processors[i][j];
			}
			offset+=box_size;
		}
	}	

	start_t=clock();


	for(i=Level-1;i>0;i--)
	{
		for(j=0;j<cnt[i];j++)
		{       
			//int father_index=mapping[i][trees[i][j].index];
			int father_index=mapping[i][j];
			struct	node *p=trees[i-1][father_index].fchild;
			if (p==NULL)
				trees[i-1][father_index].fchild=&trees[i][j];
			else
			{
				while(p->next!=NULL)
					p=p->next;
				p->next=&trees[i][j];
			}



		}


	}

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

	double max=-100000000;
	double min=100000000;

	for(i=0;i<Level;i++)
		for(j=0;j<cnt[i];j++)
		{
			if(trees[i][j].val>max)
				max=trees[i][j].val;
			if(trees[i][j].val<min)
				min=trees[i][j].val;

		}


		
		for(i=Level-1;i>0;i--)
		{
			for(j=0;j<cnt[i];j++)
			{       
				int father_index=mapping[i][j];
				trees[i][j].val=data[i][j].val-data[i-1][father_index].val;
			}

		}

	double EB=(max-min)*ratioEB;
	if(EB>10000)
		EB=100;

	sz.absErrBound = EB;

	SZ_Init_Params(&sz);
	size_t outSize;
	size_t c_size=0;
	unsigned char *bytes;

	fp_glob=data_delta;
	for(i=0;i<1;i++)
		for(j=0;j<cnt[i];j++)
		{
			struct  node *p=&trees[i][j];

			PrintTree(p,1);
		}
	size_t c_size_zfp=0;


	for(i=0;i<Level;i++)
		for(j=0;j<cnt[i];j++)
		{
			trees[i][j].flag=0;
			trees[i][j].fchild=NULL;
			trees[i][j].next=NULL;

		}

	for(i=Level-1;i>0;i--)
	{
		for(j=0;j<cnt[i];j++)
		{       
			//int father_index=mapping[i][trees[i][j].index];
			int father_index=mapping[i][j];
			if(trees[i-1][father_index].nproc==trees[i][j].nproc)
			{
				struct	node *p=trees[i-1][father_index].fchild;
				if (p==NULL)
					trees[i-1][father_index].fchild=&trees[i][j];
				else
				{
					while(p->next!=NULL)
						p=p->next;
					p->next=&trees[i][j];
				}

			}


		}


	}
	
	c_size=0;
	c_size_zfp=0;
	for(int proc=0;proc<nproc;proc++){
		int offset1=0;
		for(i=0;i<Level;i++){
			for(j=0;j<cnt[i];j++)
			{
				if(trees[i][j].nproc==proc){
					data_delta[offset1]=trees[i][j].val;
                                        offset1++;
                                }

			}
		}
		if(offset1){
			bytes    = SZ_compress(SZ_DOUBLE, data_delta, &outSize, 0, 0, 0, 0,offset1);
			c_size+=outSize;
			free(bytes);
			c_size_zfp+=zfp1D_compress (data_delta, offset1, EB );
		}
	}
	
	printf("%lf\n",(double)datasize*sizeof(double)/c_size_zfp);
	printf("%lf\n",(double)datasize*sizeof(double)/c_size);
	for(i=0;i<Level;i++)
		free(trees[i]);
	free(trees);


	free(data_delta);


	fclose(fp);

}

void box_allocate(struct box **boxes,int box_cnt[Level],int** processors,int nproc)
{

	if(nproc==0)
		return;

	int i,j;
	for(i=0;i<Level;i++)
		for(j=0;j<box_cnt[i];j++)
			processors[i][j]=0;
	int index=0;
	for(i=0;i<Level;i++)
		for(j=0;j<box_cnt[i];j++)
		{       
			if(index<nproc)

				processors[i][j]=index%nproc;

			else if(i>0) 
				processors[i][j]=processors[i-1][box_mapping[i][j].p_list[0]];
			index++;

		}



}


int main()
{
	int i,j,cnt[Level],box_cnt[Level],read_cnt;
	struct datapoint** data;
	data=malloc(Level*sizeof(* data)); 
	struct box** boxes;
	boxes=malloc(Level*sizeof(* boxes));
	mapping=malloc(Level*sizeof(* mapping));
	box_mapping=malloc(Level*sizeof(* box_mapping));
	int **processors; 
	processors=malloc(Level*sizeof(*processors));

	int list[6]={0, 1, 4, 7, 10, 15};

	for(int col=0;col<1;col++){
		char filename[50];
		sprintf(filename,"datapoint_%d",list[5]);
		
		FILE *fp=fopen(filename,"r");

		for(i=0;i<Level;i++)
		{
			read_cnt=fread(&cnt[i],sizeof(int),1,fp);
			read_cnt=fread(&box_cnt[i],sizeof(int),1,fp);
			boxes[i]=malloc(box_cnt[i]*sizeof(struct box));
			read_cnt=fread(boxes[i],sizeof(struct box),box_cnt[i],fp);
			data[i]=malloc(cnt[i]*sizeof(struct datapoint));
			read_cnt=fread(data[i],sizeof(struct datapoint),cnt[i],fp);
			processors[i]=malloc(box_cnt[i]*sizeof(int));
		}
		fclose(fp);
		
		
		for(i=0;i<Level;i++)
			mapping[i]=malloc(cnt[i]*sizeof(int));
			
		for(i=0;i<Level;i++)
			box_mapping[i]=malloc(box_cnt[i]*sizeof(struct parent_box));
			init_box_mapping(boxes,box_cnt);
		
		for(i=0;i<12;i++){
			int nproc=1<<i;
			box_allocate(boxes,box_cnt,processors,nproc);
			leveldata_box_zordering(data,cnt,boxes,box_cnt,processors,col,nproc);
			leveldata_box_zordering_level(data,cnt,boxes,box_cnt,processors,col,nproc);
		}
		
		for(i=0;i<Level;i++){	
			free(data[i]);
			free(processors[i]);
			free(boxes[i]);
			free(mapping[i]);
			free(box_mapping[i]);
		}
	}
	free(boxes);
	free(data);
	free(mapping);
	free(box_mapping);
	free(processors);
	return 1;
}
