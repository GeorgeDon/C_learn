#include"IMalloc.h"

#define align4(x) (((((x)-1)>>2)<<2)+4)

typedef struct s_block *p_block;

struct s_block{
	size_t size;
	p_block next;
	p_block prev;
	int free;
	void *ptr;
	char data[1];
};

void *base=NULL;

p_block find_block(p_block *last,size_t size){
	p_block b=base;
	while(b&&!(b->free&&b->size>=size)){
		*last=b;
		b=b->next;
	}	
	return (b);
}
#define META_DATA_SIZE 20

p_block extend_heap(p_block last,size_t s){
	p_block b;
	b=sbrk(0);
	s=align4(s);
	if(sbrk(META_DATA_SIZE+s)==(void*)-1)
		return (NULL);
	b->size=s;
	b->next=NULL;
	b->prev=last;
	b->ptr=b->data;
	if(last)
		last->next=b;
	b->free=0;
	return (b);
}

void split_block(p_block b,size_t s){
	p_block new;
	s=align4(s);
	new=b->data+s;
	new->size=b->size-s-META_DATA_SIZE;
	new->next=b->next;
	new->prev=b;
	new->free=1;
	new->ptr=new->data;
	b->size=s;
	b->next=new;
	if(new->next)
		new->next->prev=new;
}

void *Imalloc(size_t size){
	p_block b,last;
	size_t s;
	s=align4(size);
	if(base){
		last=base;
		b=find_block(&last,s);
		if(b){
			if((b->size-s)>=META_DATA_SIZE+4)
				split_block(b,s);
				b->free=0;
		}else{
			b=extend_heap(last,s);
			if(!b)
				return (NULL)	;
		}
	}else{
		b=extend_heap(NULL,s);
		if(!b)
			return (NULL)	;
		base=b;
	}
	return (b->data);
}

void *Icalloc(size_t number,size_t size){
	size_t *new;
	size_t s4,i;
	new=Imalloc(number*size);
	if(new){
		s4=align4(number*size)<<2;
		for(i=0;i<s4;i++)
			new[i]=0;
	}
	return (new);
}

//get the block from and addr
p_block get_block(void *p){
	char *temp;
	temp=p;
	return(p=temp-=META_DATA_SIZE);
}

int valid_addr(void *p){
	if(base){
		if(p>base&&p<sbrk(0))
			return (p==(get_block(p)->ptr));
	}
	return (0);
}

p_block fusion(p_block b){
	if(b->next&&b->next->free){
		b->size+=META_DATA_SIZE+b->next->size;
		b->next=b->next->next;	
		if(b->next)
			b->next->prev=b;
	}
	return (b);
}

void Ifree(void *p){
	p_block b;
	if(valid_addr(p)){
		b=get_block(p);
		b->free=1;
		if(b->prev&&b->prev->free)
			b=fusion(b->prev);
		if(b->next)
			fusion(b);
		else{
			if(b->prev)
				b->prev->next=NULL;
			else
				base=NULL;
			brk(b);
		}
	}
}

void copy_block(p_block src,p_block dst){
	int *sdata,*ddata;
	size_t i;
	sdata=src->ptr;
	ddata=dst->ptr;
	for(i=0;i*4<src->size&&i*4<dst->size;i++)
		ddata[i]=sdata[i];
}


void *Irealloc(void *p,size_t size){
	size_t s;
	p_block b,new;
	void *newp;
	if(!p)
		return (Imalloc(size));
	if(valid_addr(p)){
		s=align4(size);
		b=get_block(p);
		if(b->size>=s){
			if(b->size-s>=(META_DATA_SIZE+4))
				split_block(b,s);
		}else{
			if(b->next&&b->next->free&&
			  (b->size+META_DATA_SIZE+b->next->size)>=s){
				fusion(b);
				if(b->size-s>=(META_DATA_SIZE+4))
				split_block(b,s);
			}else{
				newp=Imalloc(s);
				if(!newp)
					return (NULL);
				new=get_block(newp);
				copy_block(b,new);
					Ifree(p);
				return(newp);
			}
		}
		return (p);
	}
	return (NULL);
}













