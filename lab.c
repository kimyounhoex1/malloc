// // /*
// //  * mm-naive.c - The fastest, least memory-efficient malloc package.
// //  *
// //  * In this naive approach, a block is allocated by simply incrementing
// //  * the brk pointer.  A block is pure payload. There are no headers or
// //  * footers.  Blocks are never coalesced or reused. Realloc is
// //  * implemented directly using mm_malloc and mm_free.
// //  *
// //  * NOTE TO STUDENTS: Replace this header comment with your own header
// //  * comment that gives a high level description of your solution.
// //  */
// // #include <stdio.h>
// // #include <stdlib.h>
// // #include <assert.h>
// // #include <unistd.h>
// // #include <string.h>

// // #include "mm.h"
// // #include "memlib.h"

// // /*********************************************************
// //  * NOTE TO STUDENTS: Before you do anything else, please
// //  * provide your team information in the following struct.
// //  ********************************************************/
// // team_t team = {
// //     /* Team name */
// //     "ateam",
// //     /* First member's full name */
// //     "Harry Bovik",
// //     /* First member's email address */
// //     "bovik@cs.cmu.edu",
// //     /* Second member's full name (leave blank if none) */
// //     "",
// //     /* Second member's email address (leave blank if none) */
// //     ""};

// // /* single word (4) or double word (8) alignment */
// // #define ALIGNMENT 8

// // /* rounds up to the nearest multiple of ALIGNMENT */
// // #define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

// // #define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// // /*수정되는 코드들*/
// // // word, header, footer size (4byte), 블록의 크기를 정의함
// // #define WSIZE 4 // word, header, footer size (4byte), 블록의 크기를 정의함
// // // double word size(8byte), 더블워드의 크기
// // #define DSIZE 8 // double word size(8byte), 더블워드의 크기
// // // init usable block size and extend heap size(4kb)
// // #define CHUNKSIZE (1<<12) // extend heap size(4kb)

// // #define MAX(x, y) ((x) > (y) ? (x) : (y))

// // // 워드 단위의 비트로 할당된 팩사이즈
// // #define PACK(size, alloc) ((size) | (alloc)) // 워드 단위의 비트로 할당된 팩사이즈

// // // 주소에 값을 읽고 쓰는 사이즈
// // #define GET(p)          (*(unsigned int *)(p))
// // #define PUT(p, val)     (*(unsigned int *)(p) = (val))

// // // 주소 p 로부터 할당된 필드의 사이즈를 읽는 변수
// // #define GET_SIZE(p)     (GET(p) & ~0x7)
// // #define GET_ALLOC(p)    (GET(p) & 0x1)

// // // block ptr = bp, header and footer size compute
// // //------------------------
// // // bp is block start point, so you want to know the hader start address, 
// // // you should minus WSIZE
// // //------------------------
// // // Header start address
// // #define HDRP(bp)        ((char *)(bp) - WSIZE)
// // // Footer start address
// // #define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

// // // next block and previous block block address
// // #define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
// // #define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

// // #define pred(bp)        ((char*)(bp) - DSIZE)
// // #define succ(bp)        ((char*)(bp) - WSIZE)

// // static char* heap_listp;
// // /**/
// // /*
// //  * mm_init - initialize the malloc package.
// //  */
// // int mm_init(void)
// // {
// //     if((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
// //         return -1;
// //     PUT(heap_listp, 0);
// //     PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));
// //     PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));
// //     PUT(heap_listp + (3*WSIZE), PACK(0, 1));
// //     heap_listp += (2*WSIZE);

// //     if(extend_heap(CHUNKSIZE/WSIZE) == NULL)
// //         return -1;
// //     return 0;
// // }

// // /*
// //  * mm_free - Freeing a block does nothing.
// //  */
// // void mm_free(void *bp)
// // {
// //     size_t size = GET_SIZE(HDRP(bp));

// //     PUT(HDRP(bp), PACK(size, 0));
// //     PUT(FTRP(bp), PACK(size, 0));
// //     coalesce(bp);
// // }

// // // static void *extend_heap(size_t words){
// // //     printf("extend_heap size: %u\n", (unsigned)words);
// // //     char *bp;
// // //     size_t size;

// // //     size = (words % 2) ? (words + 1) : words;
// // //     size *= WSIZE;
// // //     // if(words % 2 == 0)
// // //     //     size = words * WSIZE;
// // //     // else 
// // //     //     size = (words + 1) * WSIZE;
    
// // //     if((long)(bp = mem_sbrk(size)) == -1)
// // //         return NULL;
    
// // //     printf("exnted_heap words: %u\n", (unsigned)size);
// // //     printf("bp = %p\n", bp);
    
// // //     PUT(HDRP(bp), PACK(size, 0));
// // //     PUT(FTRP(bp), PACK(size, 0));
// // //     PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
    
// // //     return coalesce(bp);
// // // }

// // static void *coalesce(void *bp){
// //     // printf("before -----------------------------------\n");
// //     // printf("coalesce bp: %p\n", bp);
// //     // printf("hdr size: %u\n", GET_SIZE(HDRP(bp)));
// //     // printf("next block hdr size: %u\n", GET_SIZE(HDRP(NEXT_BLKP(bp))));
//     size_t prev_alloc;
//     if (bp == heap_listp) 
//         prev_alloc = 1;
//     else
//         prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
//     size_t next_alloc = GET_ALLOC(FTRP(NEXT_BLKP(bp)));
//     // size_t size = GET_SIZE(HDRP(bp));
//     // size_t newsize;

//     if(prev_alloc && next_alloc){
//         return bp;
//     }
//     else if(prev_alloc && !next_alloc){
//         // newsize = size + GET_SIZE(HDRP(NEXT_BLKP(bp)));
//         // PUT(HDRP(bp), PACK(newsize, 0));
//         // PUT(FTRP(bp), PACK(newsize, 0));
//         size_t new_packing = PACK(GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(bp)), 0);
//         PUT(HDRP(bp), new_packing);
//         PUT(FTRP(NEXT_BLKP(bp)), new_packing);
//     }
//     else if(!prev_alloc && next_alloc){
//         // newsize = size + GET_SIZE(HDRP(PREV_BLKP(bp)));
//         // PUT(FTRP(bp), PACK(newsize, 0));
//         // PUT(HDRP(PREV_BLKP(bp)), PACK(newsize, 0));
//         // bp = PREV_BLKP(bp);
//         size_t new_packing = PACK(GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(bp)), 0);
//         PUT(HDRP(PREV_BLKP(bp)), new_packing);
//         PUT(FTRP(bp), new_packing);
//         bp = PREV_BLKP(bp);
//     }
//     else{
//         // newsize = size + GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
//         // PUT(HDRP(PREV_BLKP(bp)), PACK(newsize, 0));
//         // PUT(FTRP(NEXT_BLKP(bp)), PACK(newsize, 0));
//         // bp = PREV_BLKP(bp);
//         size_t new_packing = PACK(GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp))), 0);
//         PUT(HDRP(PREV_BLKP(bp)), new_packing);
//         PUT(FTRP(NEXT_BLKP(bp)), new_packing);
//         bp = PREV_BLKP(bp);
//     }
//     // printf("after -----------------------------------\n");
//     // printf("coalesce bp: %p\n", bp);
//     // printf("hdr size: %u\n", GET_SIZE(HDRP(bp)));
//     // printf("next block hdr size: %u\n", GET_SIZE(HDRP(NEXT_BLKP(bp))));

//     return bp;
// // // }
// // static void *extend_heap(size_t words){
// //     char *bp;
// //     size_t size;

// //     size = (words % 2) ? (words + 1) : words;
// //     size *= WSIZE;

// //     if((long)(bp = mem_sbrk(size)) == -1)
// //         return NULL;

// //     PUT(HDRP(bp), PACK(size, 0));    
// //     PUT(FTRP(bp), PACK(size, 0));   
// //     PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
// //     // printf("exnted_heap words: %u\n", (unsigned)size);
// //     // printf("bp = %p\n", bp);

// //     return coalesce(bp);
// // }

// // // split block size
// // static void *coalesce(void *bp){
// //     size_t prev_alloc;
// //     size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
// //     size_t size = GET_SIZE(HDRP(bp));
// //     // printf("before -----------------------------------\n");
// //     // printf("coalesce bp: %p\n", bp);
// //     // printf("hdr size: %u\n", GET_SIZE(HDRP(bp)));
// //     // printf("next block hdr size: %u\n", GET_SIZE(HDRP(NEXT_BLKP(bp))));

// //     if ((char *)bp == heap_listp) {
// //         prev_alloc = 1;
// //     }
// //     else {
// //         prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
// //     }

// //     if(prev_alloc && next_alloc){
// //         return bp;
// //     }
// //     else if(prev_alloc && !next_alloc){
// //         size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
// //         PUT(HDRP(bp), PACK(size, 0));
// //         PUT(FTRP(bp), PACK(size, 0));
// //     }
// //     else if(!prev_alloc && next_alloc){
// //         size += GET_SIZE(HDRP(PREV_BLKP(bp)));
// //         PUT(FTRP(bp), PACK(size, 0));
// //         PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
// //         bp = PREV_BLKP(bp);
// //     }
// //     else{
// //         size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
// //         PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
// //         PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
// //         bp = PREV_BLKP(bp);
// //     }
// //     // printf("after -----------------------------------\n");
// //     // printf("coalesce bp: %p\n", bp);
// //     // printf("hdr size: %u\n", GET_SIZE(HDRP(bp)));
// //     // printf("next block hdr size: %u\n", GET_SIZE(HDRP(NEXT_BLKP(bp))));

// //     return bp;
// // }


// // /*
// //  * mm_malloc - Allocate a block by incrementing the brk pointer.
// //  *     Always allocate a block whose size is a multiple of the alignment.
// //  */

// //  void *first_fit(size_t asize, void *bp){
// //     if(GET_SIZE(HDRP(bp)) == 0){
// //         return NULL;
// //     }
// //     if (!GET_ALLOC(HDRP(bp)) && (GET_SIZE(HDRP(bp)) >= asize))
// //         return bp;
// //     return first_fit(asize, NEXT_BLKP(bp));
// //  }

// //  static void *find_fit(size_t asize)
// //  {
// //     // 헤더들을 참조해서, alloc이 1인 경우인 애들만 찾아서 블록 크기 비교
// //     return first_fit(asize, heap_listp);
// //     // void *bp;
// //     // for(bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)){
// //     //     if(!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))){
// //     //         return bp;
// //     //     }
// //     // }

// //     return NULL;
// //  }
 
// //  /* split block size into free and allocated */
// //  static void place(void *bp, size_t asize){
// //     // 블록 재배치
// //     // size_t prev_size = GET_SIZE(HDRP(bp));
    
// //     // PUT(HDRP(bp), PACK(asize, 1));
// //     // PUT(HDRP(bp) + asize, PACK(asize, 1));

// //     // PUT(HDRP(bp)+asize, PACK(prev_size - asize, 0));
// //     // PUT(NEXT_BLKP(bp), PACK(prev_size - asize, 0));
// //     size_t csize = GET_SIZE(HDRP(bp));
// //     if ((csize-asize) >= (2*DSIZE)){
// //         PUT(HDRP(bp), PACK(asize,1));
// //         PUT(FTRP(bp), PACK(asize,1));
// //         bp = NEXT_BLKP(bp);
// //         PUT(HDRP(bp), PACK(csize-asize,0));
// //         PUT(FTRP(bp), PACK(csize-asize,0));
// //     }
// //     else{
// //         PUT(HDRP(bp), PACK(csize,1));
// //         PUT(FTRP(bp), PACK(csize,1));
// //     }
// //  }


// // void *mm_malloc(size_t size)
// // {
// //     // int newsize = ALIGN(size + SIZE_T_SIZE);
// //     // void *p = mem_sbrk(newsize);
// //     // if (p == (void *)-1)
// //     //     return NULL;
// //     // else
// //     // {
// //     //     *(size_t *)p = size;
// //     //     return (void *)((char *)p + SIZE_T_SIZE);
// //     // }
// //     size_t asize;
// //     size_t extendsize;
// //     char *bp;

// //     if (size == 0) return NULL;

// //     if (size <= DSIZE){
// //         asize = 2*DSIZE;
// //     }
// //     else {
// //         asize = DSIZE* ( (size + (DSIZE)+ (DSIZE-1)) / DSIZE );
// //     }

// //     if ((bp = find_fit(asize)) != NULL){
// //         place(bp,asize);
// //         return bp; 
// //     }

// //     extendsize = MAX(asize,CHUNKSIZE);
// //     if ((bp=extend_heap(extendsize/WSIZE)) == NULL){ 
// //         return NULL;
// //     }
// //     place(bp,asize);
// //     return bp;
// // }


// // /*
// //  * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
// //  */
// // void *mm_realloc(void *ptr, size_t size)
// // {
// //     void *oldptr = ptr;
// //     void *newptr;
// //     size_t copySize;

// //     newptr = mm_malloc(size);
// //     if (newptr == NULL)
// //         return NULL;
// //     copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
// //     if (size < copySize)
// //         copySize = size;
// //     memcpy(newptr, oldptr, copySize);
// //     mm_free(oldptr);
// //     return newptr;
// // }