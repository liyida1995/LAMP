#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fpzip.h"
#include "fpc1.h"

#define Level 6
#define Refine_ratio 2






int * fp_glob;
int ** mapping;

struct parent_box{
	int num;
	int p_list[20];
	int offset[20];
};
struct parent_box **box_mapping;
int Part1By1(int x);
int EncodeMorton2(int x, int y)
{
	return (Part1By1(y) << 1) + Part1By1(x);
}


// "Insert" a 0 bit after each of the 16 low bits of x
int Part1By1(int x)
{
	x &= 0x0000ffff;                  // x = ---- ---- ---- ---- fedc ba98 7654 3210
	x = (x ^ (x <<  8)) & 0x00ff00ff; // x = ---- ---- fedc ba98 ---- ---- 7654 3210
	x = (x ^ (x <<  4)) & 0x0f0f0f0f; // x = ---- fedc ---- ba98 ---- 7654 ---- 3210
	x = (x ^ (x <<  2)) & 0x33333333; // x = --fe --dc --ba --98 --76 --54 --32 --10
	x = (x ^ (x <<  1)) & 0x55555555; // x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
	return x;
}



// Inverse of Part1By1 - "delete" all odd-indexed bits
int Compact1By1(int x)
{
	x &= 0x55555555;                  // x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
	x = (x ^ (x >>  1)) & 0x33333333; // x = --fe --dc --ba --98 --76 --54 --32 --10
	x = (x ^ (x >>  2)) & 0x0f0f0f0f; // x = ---- fedc ---- ba98 ---- 7654 ---- 3210
	x = (x ^ (x >>  4)) & 0x00ff00ff; // x = ---- ---- fedc ba98 ---- ---- 7654 3210
	x = (x ^ (x >>  8)) & 0x0000ffff; // x = ---- ---- ---- ---- fedc ba98 7654 3210
	return x;
}




int DecodeMorton2X(int code)
{
	return Compact1By1(code >> 0);
}

int DecodeMorton2Y(int code)
{
	return Compact1By1(code >> 1);
}

void rot(int n, int *x, int *y, int rx, int ry) {
	if (ry == 0) {
		if (rx == 1) {
			*x = n-1 - *x;
			*y = n-1 - *y;
		}

		//Swap x and y
		int t  = *x;
		*x = *y;
		*y = t;
	}
}
int xy2d (int n, int x, int y) {
	int rx, ry, s, d=0;
	for (s=n/2; s>0; s/=2) {
		rx = (x & s) > 0;
		ry = (y & s) > 0;
		d += s * s * ((3 * rx) ^ ry);
		rot(n, &x, &y, rx, ry);
	}
	return d;
}



struct datapoint{
	int a,b;
	double val;
};

struct box{
	int x1,y1;
	int x2,y2;
};

struct node{
	struct node *fchild;
	struct node *next;
	int index;
	int ori_index;
};

/* compress doubleing-point data */
	static size_t
compress(FPZ* fpz, const void* data)
{
	size_t size;
	/* write header */
	if (!fpzip_write_header(fpz)) {
		fprintf(stderr, "cannot write header: %s\n", fpzip_errstr[fpzip_errno]);
		return 0;
	}
	/* perform actual compression */
	size = fpzip_write(fpz, data);
	if (!size) {
		fprintf(stderr, "compression failed: %s\n", fpzip_errstr[fpzip_errno]);
		return 0;
	}
	return size;
}

/* perform compression, decompression, and validation of double array */
	static size_t
fpzip_double_array(const double* field, int nx, int ny, int nz, int prec)
{
	int status;
	int dims = (nz == 1 ? ny == 1 ? 1 : 2 : 3);
	size_t inbytes = nx * ny * nz * sizeof(double);
	size_t bufbytes = 1024 + inbytes;
	size_t outbytes = 0;
	void* buffer = malloc(bufbytes);
	char name[0x100];

	/* compress to memory */
	FPZ* fpz = fpzip_write_to_buffer(buffer, bufbytes);
	fpz->type = FPZIP_TYPE_DOUBLE;
	fpz->prec = prec;
	fpz->nx = nx;
	fpz->ny = ny;
	fpz->nz = nz;
	fpz->nf = 1;
	outbytes = compress(fpz, field);
	status = (0 < outbytes && outbytes <= bufbytes);
	fpzip_write_close(fpz);

	free(buffer);

	return outbytes;
}


void PrintTree(struct node* p,  int flag)
{
	if (p==NULL) 
		return;
	if(flag==1)
		fp_glob[0]= p->ori_index;
	else
	{
		//		if(fp_glob[0]-(*p).val>10)
		//			printf("%lf,%lf\n",(*p).val,fp_glob[0]);

		p->ori_index=fp_glob[0];

	}
	fp_glob++;
	//p->flag=1;
	if (p->fchild!=NULL) 
	{ PrintTree(p->fchild,flag); }
	if (p->next!=NULL)
	{ PrintTree(p->next,flag); } 


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
	struct box *overlap=(struct box *)malloc(sizeof(struct box));

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
			/*Child box is overlaped with only one parent box*/ 
			for( k=0;k<box_cnt[i-1];k++)
				if(compare_box(boxes[i][j],boxes[i-1][k]))
				{
					box_mapping[i][j].num=1;
					box_mapping[i][j].p_list[0]=k;
					box_mapping[i][j].offset[0]=offset;

					break;

				}
				else

					offset=offset+(boxes[i-1][k].x2-boxes[i-1][k].x1+1)*(boxes[i-1][k].y2-boxes[i-1][k].y1+1);


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
						//printf("%d,%d:%d\n",i,j,l);
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

		}//j loop 




	}


	/*for(i=Level-1;i>0;i--)
	  for(j=0;j<cnt[i];j++)
	  {
	  if(data[i][j].a/Refine_ratio!=data[i-1][mapping[i][j]].a||data[i][j].b/Refine_ratio!=data[i-1][mapping[i][j]].b) 
	  printf("Error! %d,%d: %d, %d\n",data[i][j].a,data[i-1][mapping[i][j]].a,data[i][j].b,data[i-1][mapping[i][j]].b);
	  }*/

}





void  get_baseline_encode_recipe(int *encode_recipe, int cnt [Level],struct box **boxes,int box_cnt[Level])
{
	//	clock_t start_t, end_t; double total_t;

	//	start_t = clock();
	int i,j,k;

	int datasize=0;
	for(i=0;i<Level;i++)
		datasize=datasize+cnt[i];
	int offset=0;
	int level_offset=0;
	for(i=0;i<Level;i++)
	{ 
		offset=0;
		for(j=0;j<box_cnt[i];j++)
		{
			int a=boxes[i][j].y2-boxes[i][j].y1;
			int b=boxes[i][j].x2-boxes[i][j].x1;
			int z_size= EncodeMorton2(a,b)+1;
			int *z_index=(int *)malloc(z_size*sizeof(int));
			for(k=0;k<z_size;k++){
				z_index[k]=-1;
			}

			int box_size=(a+1)*(b+1);

			for(int m=0;m<=b;m++)
				for(int l=0;l<=a;l++)
				{
					z_index[EncodeMorton2(l,m)]=l+m*(a+1);
				}
			int *recipe_en=(int *)malloc(box_size*sizeof(int));
			int tr=0;
			for(k=0;k<z_size;k++){
				if(z_index[k]!=-1)
					recipe_en[tr++]=z_index[k];
			}
			if(tr!=box_size){
				printf("tr!=box_size, %d, %d\n",tr,box_size);
				for (k=0;k<box_size;k++)
					printf("%d ",recipe_en[k]);
			}
			for(k=0;k<box_size;k++)
			{
				encode_recipe[level_offset+offset+k]=level_offset+offset+recipe_en[k];
			}

			offset+=box_size;
			free(z_index);
			free(recipe_en);
		}

		level_offset+=cnt[i];
	}

	//end_t = clock();
	//total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
	//printf("get_baseline_encode_recipe: %lf\n", total_t  );




}

void get_levelRe_encode_recipe(struct datapoint **data,int *encode_recipe, int cnt [Level],struct box **boxes,int box_cnt[Level])
{
	//	clock_t start_t, end_t; double total_t;
	//	start_t = clock();
	int i,j,k;
	FILE *fp;
	struct node** trees;
	trees=(struct node**)malloc(Level*sizeof(* trees));
	for(i=0;i<Level;i++)
		trees[i]=(struct node*)malloc(cnt[i]*sizeof(struct node));
	int datasize=0;
	for(i=0;i<Level;i++)
		datasize=datasize+cnt[i];

	mapping_by_box(data,cnt,boxes,box_cnt);
	int offset=0;



	int* recipe_de;
	int level_offset=0;
	for(i=0;i<Level;i++)
	{ 
		offset=0;

		if(i<Level-1)
			recipe_de=(int*)malloc(cnt[i]*sizeof(int));
		for(j=0;j<box_cnt[i];j++)
		{
			int a=boxes[i][j].y2-boxes[i][j].y1;
			int b=boxes[i][j].x2-boxes[i][j].x1;
			int z_size= EncodeMorton2(a,b)+1;
			int *z_index=(int*)malloc(z_size*sizeof(int));
			for(k=0;k<z_size;k++){
				z_index[k]=-1;
			}

			int box_size=(a+1)*(b+1);

			for(int m=0;m<=b;m++)
				for(int l=0;l<=a;l++)
				{
					z_index[EncodeMorton2(l,m)]=l+m*(a+1);
				}
			int *recipe_en=(int *)malloc(box_size*sizeof(int));
			int tr=0;
			for(k=0;k<z_size;k++){
				if(z_index[k]!=-1)
					recipe_en[tr++]=z_index[k];
			}
			if(tr!=box_size){
				printf("tr!=box_size, %d, %d\n",tr,box_size);
				for (k=0;k<box_size;k++)
					printf("%d ",recipe_en[k]);
			}
			for(k=0;k<box_size;k++)
			{
				trees[i][offset+k].index=offset+recipe_en[k];
				trees[i][offset+k].fchild=NULL;
				trees[i][offset+k].next=NULL;
				trees[i][offset+k].ori_index=level_offset+offset+recipe_en[k];
			}
			if(i<Level-1){

				for(int j1=0;j1<box_size;j1++){
					recipe_de[recipe_en[j1]+offset]=j1+offset;
				}
			}

			offset+=box_size;
			free(z_index);
			free(recipe_en);
		}

		if(i<Level-1)
		{
			for(j=0;j<cnt[i+1];j++){
				mapping[i+1][j]=recipe_de[mapping[i+1][j]];
			}

			free(recipe_de);

		}
		level_offset+=cnt[i];
	}


	/*	   for(i=Level-1;i>0;i--)
		   for(j=0;j<cnt[i];j++)
		   {
		   if(trees[i][j].x/Refine_ratio!=trees[i-1][mapping[i][trees[i][j].index]].x||trees[i][j].y/Refine_ratio!=trees[i-1][mapping[i][trees[i][j].index]].y)
		   printf("error! %d,%d: %d, %d\n",trees[i][j].x,trees[i-1][mapping[i][trees[i][j].index]].x,trees[i][j].y,trees[i-1][mapping[i][trees[i][j].index]].y);
		   }

*/


	for(i=Level-1;i>0;i--)
	{
		for(j=0;j<cnt[i];j++)
		{       
			int father_index=mapping[i][trees[i][j].index];
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

	fp_glob=encode_recipe;
	for(i=0;i<1;i++)
		for(j=0;j<cnt[i];j++)
		{
			struct  node *p=&trees[i][j];
			PrintTree(p,1);

		}


	for(i=0;i<Level;i++)
		free(trees[i]);
	free(trees);

	//	end_t = clock();
	//	total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
	//	printf("get_levelRe_encode_recipe: %lf\n", total_t  );

}



union HEXDOUBLE {
    double num;
    unsigned long long data;
};



int main(int argc, char **argv)
{
	int i,j,cnt[Level],box_cnt[Level],read_cnt;
	struct datapoint** data;
	union HEXDOUBLE temp1,temp2;
	data=(struct datapoint**)malloc(Level*sizeof(* data)); 
	struct box** boxes;
	boxes=(struct box**)malloc(Level*sizeof(* boxes));

	int list[6]={0,2,4,7,10,15}; 
	char filename[50];
	sprintf(filename,"./inputData/Green/datapoint_%d",list[0]);
        
        FILE *fout;
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
		boxes[i]=(struct box *)malloc(box_cnt[i]*sizeof(struct box));
		read_cnt=fread(boxes[i],sizeof(struct box),box_cnt[i],fp);
		data[i]=(struct datapoint*)malloc(cnt[i]*sizeof(struct datapoint));
		read_cnt=fread(data[i],sizeof(struct datapoint),cnt[i],fp);
		//for(j=0;j<cnt[i];j++)
		//	printf("%d %d %lf\n", data[i][j].a,data[i][j].b,data[i][j].val);
	}
	fclose(fp);

	mapping=(int**)malloc(Level*sizeof(* mapping));
	for(i=0;i<Level;i++)
		mapping[i]=(int*)malloc(cnt[i]*sizeof(int));

	box_mapping=(struct parent_box**)malloc(Level*sizeof(* box_mapping));
	for(i=0;i<Level;i++)
		box_mapping[i]=(struct parent_box*)malloc(box_cnt[i]*sizeof(struct parent_box));




	//	int* recipe=	get_baseline_encode_recipe_hilbert(data,cnt,boxes,box_cnt);
	//	free(recipe);
	int datasize=0;
	for(i=0;i<Level;i++)
		datasize=datasize+cnt[i];
	union HEXDOUBLE *data_delta=(union HEXDOUBLE*)malloc(datasize*sizeof(double));
	union HEXDOUBLE *data_level=(union HEXDOUBLE*)malloc(datasize*sizeof(double));
	double *data_baseline=(double*)malloc(datasize*sizeof(double));
	int *   recipe_en_levelRe=(int*)malloc(datasize*sizeof(int));
	int *   recipe_en_baseline=(int*)malloc(datasize*sizeof(int));



	//get_levelRe_encode_recipe(data,recipe_en_levelRe,cnt,boxes,box_cnt);
	//get_baseline_encode_recipe( recipe_en_baseline,cnt,boxes,box_cnt);

 mapping_by_box(data,cnt,boxes,box_cnt);



	for(i=0;i<Level;i++)	
		free(data[i]);
	for(i=0;i<Level;i++)
		free(boxes[i]);
	for(int col=0;col<6;col++){
		sprintf(filename,"./inputData/Green/datapoint_%d",list[col]);

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
			//for(j=0;j<cnt[i];j++)
			//	printf("%d %d %lf\n", data[i][j].a,data[i][j].b,data[i][j].val);
		}
		fclose(fp);

		int offset=0;
		for(i=0;i<Level;i++)
			for(j=0;j<cnt[i];j++)
			{
				data_level[offset].num=data[i][j].val;
				data_delta[offset].num=data[i][j].val;
				offset++;

			}

		int c_size_fpc;
		int c_size_fpzip;

		fp=fopen("ori","w");
		if(fp==NULL){
			printf("Cannot open file %s.fpc\n",filename);
			return 0;
		}
		fwrite(data_level,datasize, sizeof(double),fp);
		fclose(fp);


		fp=fopen("ori","r");
		if(fp==NULL){
			printf("Cannot open file %s.fpc\n",filename);
			return 0;
		}
                sprintf(filename,"ori_Green_%d.count",list[col]);
                fout=fopen(filename,"w");
                if(fp==NULL){
                        printf("Cannot open file %s.fpc\n","ori.count");
                        return 0;
                }

		c_size_fpc=FPC_Compress_count(20,fp,fout);
		fclose(fp);
		fclose(fout);
		printf("fpc ori compression ratio=%lf\n",(double)datasize*sizeof(double)/c_size_fpc);
	//	c_size_fpzip=fpzip_double_array(data_level, datasize, 1, 1, 64);
	//	printf("fpzip ori compression ratio=%lf\n",(double)datasize*sizeof(double)/c_size_fpzip);

		int level_offset=datasize;
	for(i=Level-1;i>0;i--)
	{
		level_offset=level_offset-cnt[i];
		for(j=0;j<cnt[i];j++)
		{       
			
			int father_index=mapping[i][j];
                        //data_delta[ level_offset+j]=data[i][j].val-data[i-1][father_index].val;
                       // data_delta[ level_offset+j]=((*(unsigned long *)&data[i][j].val) ^ (*(unsigned long *)&data[i-1][father_index].val));
		       temp1.num=data[i][j].val;
		       temp2.num=data[i-1][father_index].val;
                        data_delta[ level_offset+j].data=temp1.data^temp2.data;



		}


	}

		fp=fopen("ori+","w");
		if(fp==NULL){
			printf("Cannot open file %s.fpc\n",filename);
			return 0;
		}
		fwrite(data_delta,datasize, sizeof(double),fp);
		fclose(fp);


		fp=fopen("ori+","r");
		if(fp==NULL){
			printf("Cannot open file %s.fpc\n",filename);
			return 0;
		}
               
	       	sprintf(filename,"LAMP_Green_%d.count",list[col]);
                fout=fopen(filename,"w");

                if(fp==NULL){
                        printf("Cannot open file %s.fpc\n","LAMP.count");
                        return 0;
                }

		c_size_fpc=FPC_Compress_count(20,fp,fout);
		fclose(fp);
		fclose(fout);
		printf("fpc ori+ compression ratio=%lf\n",(double)datasize*sizeof(double)/c_size_fpc);
	//	c_size_fpzip=fpzip_double_array(data_delta, datasize, 1, 1, 64);
	//	printf("fpzip ori+ compression ratio=%lf\n",(double)datasize*sizeof(double)/c_size_fpzip);





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

