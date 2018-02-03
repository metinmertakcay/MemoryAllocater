/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines.
 */

#include "mm_alloc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

#define SIZE sizeof(struct metadata)

struct metadata *head = NULL;	//linkli liste yapisi kullanilacak. Baslangic adresini tutan degisken

/*Metadata bilgilerini tutacak olan degisken*/
struct metadata{
	struct metadata *next;		//Kendisinden sonra gelen metadataları degisken
	struct metadata *prev;		//Kendisinden once gelen metadataları degisken
	size_t size;				//Block size
	int free;					//Kullanilabilir olma durumunu belirleyen degisken
};

/*Bu fonksiyon daha onceden olusturulmus olan linkli listede uygun bir bolge olup olmadigini kontrol eder.
@param size: Acilmak istenen yerin boyutu*/
struct metadata *get_free_block(size_t size)
{
	//Linkli liste uzerinde teker teker gezme islemi yapilacaktir.
	struct metadata *current = head;	//Linkli liste üzerinde dolasilmasi islemi icin olusturulmustur.
	struct metadata *splitted_block;	//Ayrilabilecek boyutta olan blogu temsil edecek olan degisken
	size_t remaining;					//Kalan alan bilgisinin tutulmasi icin olsturulmus degisken
	//Null karakteri gorunene kadar arama islemi yapilacaktir.
	while(current != NULL)
	{
		//Bos olan ve uygun buyuklukte olan ilk yer belirlenecektir
		if((current->free == 1)&&(current->size > size))
		{
			//Eger blok cok buyukse(icerisine bir tane daha metadata bilgisi alabiliyorsa) bu kisim ayri bir blok olarak ayrilir.
			if(current->size - size > sizeof(struct  metadata))
			{
				//Diger blok icin kullanilabilecek olan alan belirleniyor.
				remaining = current->size - size - sizeof(struct metadata);
				//Suanki blogun kullandigi boyut bilgisi degistiriliyor.
				current->size = size;
				splitted_block = (void *) (current + 1) + size;
				//Ayrilmis olan bloga ait metadata bilgisileri ataniyor
				splitted_block->free = 1;
				//Kullanilabilecek alan belirleniyor
				splitted_block->size = remaining;
				//Linkli listeye eklenme islemi gerceklestiriliyor
				splitted_block->prev = current;
				splitted_block->next = current->next;
				current->next->prev  = splitted_block;
				current->next = splitted_block;
			}
			//Eger boyle bir yer varsa bu bilgi geriye dondurulur.
			return current;
		}
		current = current->next;
	}
	return NULL;
}

//This function prints the information about allocated memory which exist inside struct metadata 
void mm_print_mem()
{
    int i;
    struct metadata *current = head;

    printf("\n\nHEAD OF LINKED LIST %ld",(long unsigned int)head);
    printf("\n\nstart_adr\tsize\tfree\tprev\tnext");
    printf("\n--------------------------------------");
    printf("\n--------------------------------------");
    while((current != NULL) && (i <= 10))
    {
        printf("\n%ld\t%zu\t%d\t%ld\t%ld",(long unsigned int)current,current->size,current->free,(long unsigned int)current->prev,(long unsigned int)current->next);
        if(current->next == NULL)
        {
            break;
        }
        current = current->next;
        i++;
    }
}

/*Bu fonksiyon blogun sifirlar ile doldurulmasi icin olusturulmustur.
@param block: Sifirlarla doldurulacak olan allocated block
@param size: Acilmis olan yerin boyutu*/
void fill_zero(void *block,size_t size)
{
	//Sifirlama islemi gerceklestiriliyor.
	memset(block,0,size*sizeof(char));
}

/*Bu fonksiyon bellekte dinamik olarak yer acilmasini saglar
@param size: Acilmak istenen boyut.
return: Acilmis olan yer geriye dondurulur.*/
void *mm_malloc(size_t size) {
	struct metadata *mdata, *current, *previous;
    void *allocated_block;
    size_t total;

    //Acilmak istenen miktar 0 ise Null geriye dondurulur.
    if(size == 0)
    {
    	return NULL;
    }
    else
    {
    	mdata = get_free_block(size);
    	//Daha once acilmis olan metadata bloklardan bos ve kullanilabilir olan bir yer olup olmadigi kontrol ediliyor.
    	if(mdata != NULL)
    	{
    		//Eger boyle bir bolge varsa kullanilabilir olma durumu kullanilamaz olarak belirleniyor.
    		mdata->free = 0;
    		fill_zero((void *) (mdata + 1),size);
    		return (void *) (mdata + 1);
    	}
    	else
    	{
    		//Genisletilecek olan miktar hesaplaniyor.
    		total = sizeof(struct metadata) + size;
    		//sbrk() sistem cagrisi kullanilarak genisletme gerceklestiriliyor.
    		allocated_block = sbrk(total);
    		//Herhangi bir nedenden dolayi allocate edilememesi durumudur.
    		if(allocated_block == (void *) -1)
    		{
    			return NULL;
    		}
    		else
    		{
    			//Acilmis olan block bilgisi saklaniyor.
    			mdata = allocated_block;
    			//Boyutu yerlestiriliyor.
    			mdata->size = size;
    			//Kullanilacagi icin free 0 olarak ayarlaniyor.
    			mdata->free = 0;
    			//Geri ve ileri gostericileri Null ataniyor.
    			mdata->next = NULL;
    			mdata->prev = NULL;
    			//ilk yer acilmasi olup olmadigi kontrol ediliyor.
    			if(head == NULL)
    			{
    				//Eger ilk yer acilmasiysa acilan ilk blok baslangic olarak atanir.
    				head = mdata;
    			}
    			else
    			{
    				//Eger daha once metadata eklenmis ise yeni olusturulmus olan bu blok ile eklenmis olan bloklar arasinda baglanti kurulur.
    				previous = head;
    				current = head->next;
    				//Ekleme yeri ilk Null gorunen yer olacaktir.
    				while(current != NULL)
    				{
    					previous = current;
    					current = current->next;
    				}
    				//Bulunan yere olusturulmus olan metadata ekleniyor.
    				previous->next = mdata;
    				mdata->prev = previous;
    			}
    		}
    	}
    	fill_zero((void *) (mdata + 1),size);
    	//Metadata bolgesi gecilerek kullanilabilir alanin adresi geriye donduruluyor.
    	return (void *) (mdata + 1);
	}
}

/*Bu fonksiyon kullanilan alanin boyutunun arttirilmasi icin olusturulmus olan degiskendir.
@param ptr: İcerisinde kullanici bilgilerinin oldugu, genisletilmek istenen alan
@param size: Genisletilmek istenen alan
retur: genisletilmis olan kullanilabilir alan(dogru bir sekilde genisletilme istenmisse) dondurulur*/
void *mm_realloc(void *ptr, size_t size) {
    struct metadata *mdata;
    void *new_ptr;

    //Pointer degeri null ise
    if(ptr == NULL)
    {
    	//ve istenen boyut 0'dan farkli ise dinamik olarak bellekten yer ayrilir
    	if(size != 0)
    	{
    		//printf("\nRealloc icinde yer acma islemi olmustur");
    		return mm_malloc(size);
    	}
    	else
    	{
    		//Eger boyut 0 ise geriye Null dondurulur
    		return NULL;
    	}
    }
    else
    {
    	//Var olan bir pointer girilmesi ancak boyutun 0 girilmesi durumunda alan free edilir
    	if(size == 0)
    	{
    		mm_free(ptr);
    		return NULL;
    	}
    	else
    	{
    		//Pointer ve size degiskenlerinin dogru girilmesi durumunda metadata bilgisi elde edilerek diger kontroller gerceklestirilir.
    		mdata = (struct  metadata *)(ptr - SIZE);			
    		//Var olan alan daraltilmaya,azaltilmaya calisiliyorsa eski acilmis olan yer geriye dondurulur.
    		if(mdata->size >= size)
    		{
    			return ptr;
    		}
    		else
    		{
    			//Genisletme islemi icin bellekten yer ayrilir.
    			new_ptr =  mm_malloc(size);
    			//Yerin acilip acilmadigi kontrol edilir.
    			if(new_ptr != NULL)
    			{
    				//Eski tarafta bulunan bilgiler kopyalanir ve eski yer free edilir.
    				memcpy(new_ptr,ptr,mdata->size);
    				mm_free(ptr);
    				return new_ptr;
    			}
    			else
    			{
    				return ptr;
    			}
    		}
    	}
    }
}

/*Bu fonksiyon ardisik olan bos adreslerin birlestirilmesi islemini gerceklestirir.
@param struct metadata *metadata: Ardisik komsu metadatalarina bakilacak olan metadata*/
void coalesceConsecutiveFreeBlock(struct metadata *mdata)
{
	struct metadata *current = mdata;		//Linkli liste uzerinde onceki ve sonraki metadataya gitmeyi saglayacak olan degisken
	size_t newSize;							//Kullanilabilir yeni alan

	//Free edilen alanin basinde ve sonrasinda bos olan bir alan varsa bu bos kisimlar birlestirilir ve tek bir bos alan elde edilir.
	if((current->prev != NULL)&&(current->next != NULL))
	{
		if((current->next->free == 1)&&(current->prev->free == 1))
		{
			//Yeni olusacak bos alan hesaplaniyor
			newSize = current->prev->size + sizeof(struct metadata) + current->size + sizeof(struct metadata) + current->next->size;
			//Fazlalik olan ve bos bolgeyi gosteren metadatalar linkli liste ile ulasilmasi engelleniyor.
			current->prev->next = current->next->next;
			current->next->next->prev = current->prev;
			current->prev->size = newSize;				//Yeni elde edilmis boyut metadataya ataniyor.
		}
		else if(current->next->free == 1)
		{
			//Sadece ileri yondeki metadatanin bos olmasi durumudur.
			//Toplam kullanilabilecek alan elde ediliyor.
			newSize = current->size + sizeof(struct metadata) + current->next->size;
			//Linkli liste organize ediliyor.
			current->next->next->prev = current;
			current->next = current->next->next;
			current->size = newSize;
		}
		else if(current->prev->free == 1)
		{
			//Sadece geri yondeki metadatada bos alanin olmasi durumudur.
			newSize = current->prev->size + sizeof(struct metadata) + current->size;
			current->next->prev = current->prev;
			current->prev->next = current->next;
			current->prev->size = newSize;
		}
	}
	else if(current->next != NULL)
	{
		if(current->next->free == 1)
		{
			//Sadece ileri yondeki metadatanin bos olmasi durumudur.
			//Toplam kullanilabilecek alan elde ediliyor.
			newSize = current->size + sizeof(struct metadata) + current->next->size;
			//Linkli liste organize ediliyor.
			current->next->next->prev = current;
			current->next = current->next->next;
			current->size = newSize;
		}
	}
	else if(current->prev != NULL)
	{
		if(current->prev->free == 1)
		{
			//Sadece geri yondeki metadatada bos alanin olmasi durumudur.
			newSize = current->prev->size + sizeof(struct metadata) + current->size;
			current->next->prev = current->prev;
			current->prev->next = current->next;
			current->prev->size = newSize;
		}
	}
	return;
}

/*Bu fonksiyon dinamik olarak acilmis olan bellegin iade edilmesi icin olusturulmustur
@param ptr: Yeri free edilecek olan pointer*/
void mm_free(void *ptr) {
    struct metadata *mdata;
    //Acilmamis olan yer free edilmeye calisilirsa herhangi bir sey yapilmaz.
    if(ptr == NULL)
    {
    	return;
    }
    else
    {
    	//Metadata bilgisi elde edilir. Elde edilen metadata kullanilabilir hale gelir.
    	mdata = (struct  metadata *)(ptr - SIZE);
    	mdata->free = 1;
    	//Blok birlestirme islemi yapilir.
    	coalesceConsecutiveFreeBlock(mdata);
    	return;
    }
}
