#include <ap_int.h>
#include <hls_stream.h>
#include <data_types.h>

#ifndef __DPATH_H__
#define __DPATH_H__


// Following are the main functions used to create data path
// Two Dimensional TriDiagonal Solvers in Batched Mode
template <bool FPPREC>
static void read_dat(
		const uint512_dt*d, hls::stream<uint256_dt> &d_stm,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B);

template <bool FPPREC>
static void read_dat512(
		const uint512_dt*d, hls::stream<uint512_dt> &d_stm,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B);

template<bool FPPREC>
static void read_plane(const uint512_dt*d, hls::stream<uint256_dt> &d_stm,
		ap_uint<9> M, ap_uint<9> N, ap_uint<9> L, ap_uint<10> B, unsigned char dirXYZ);

template<bool FPPREC>
static void read_plane512(const uint512_dt*d, hls::stream<uint512_dt> &d_stm,
		ap_uint<9> M, ap_uint<9> N, ap_uint<9> L, ap_uint<10> B, unsigned char dirXYZ);

template<bool FPPREC>
static void read_Tile512(
		const uint512_dt*d, hls::stream<uint512_dt> &d_stm,
		ap_uint<12> M, ap_uint<12> N, ap_uint<8> B, unsigned char dirXYZ);

static void FIFO_256_512(hls::stream<uint256_dt> &stm_in, 
	hls::stream<uint512_dt> &stm_out, int total_512);


static void FIFO_512_256(hls::stream<uint512_dt> &stm_in, 
	hls::stream<uint256_dt> &stm_out, int total_512);

static void HBM_fifo(uint512_dt*HBM_buffer1, uint512_dt*HBM_buffer2, 
	hls::stream<uint512_dt> &stm_in, hls::stream<uint512_dt> &stm_out,
		int count_w, int count_b, int count_r);

template <bool FPPREC, int DMAX>
static void interleaved_row_block8(hls::stream<uint256_dt> &stm_in, 
	hls::stream<uint256_dt> &stm_out,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B, bool interleave);

template <bool FPPREC, int DMAX>
static void row2col(hls::stream<uint256_dt> &in, hls::stream<uint256_dt> &out,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B);

template <bool FPPREC, int DMAX>
static void undo_interleaved_row_block8(hls::stream<uint256_dt> &stm_in, hls::stream<uint256_dt> &stm_out,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B, bool undo_interleave);

template <bool FPPREC, int DMAX>
static void col2row(hls::stream<uint256_dt> &in, hls::stream<uint256_dt> &out0,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B);

template <bool FPPREC, class DType>
static void stream_8x8transpose(hls::stream<uint256_dt> &in, hls::stream<uint256_dt> &out0,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B, bool transpose);

template <bool FPPREC>
static void write_dat(uint512_dt* u, hls::stream<uint256_dt> &u_stm,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B);

template <bool FPPREC>
static void write_dat512(uint512_dt* u, hls::stream<uint512_dt> &u_stm,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B);

template<bool FPPREC>
static void write_plane(uint512_dt* u, hls::stream<uint256_dt> &u_stm,
		ap_uint<9> M, ap_uint<9> N, ap_uint<9> L, ap_uint<10> B, unsigned char dirXYZ);

template<bool FPPREC>
static void write_plane512(uint512_dt* u, hls::stream<uint512_dt> &u_stm,
		ap_uint<9> M, ap_uint<9> N, ap_uint<9> L, ap_uint<10> B, unsigned char dirXYZ);

template<bool FPPREC>
static void write_Tile512(uint512_dt* u, hls::stream<uint512_dt> &u_stm,
		ap_uint<12> M, ap_uint<12> N, ap_uint<12> B, unsigned char dirXYZ);


template<bool FPPREC, class DType>
static void vec_to_Dtype(hls::stream<uint256_dt> &stm_vec_in, hls::stream<DType> &stm_scl_out, int size);

template<bool FPPREC, class DType>
static void Dtype_to_vec(hls::stream<DType> &stm_scl_in, hls::stream<uint256_dt> &stm_vec_out, int size);


// TDMA Modules
template <bool FPPREC>
static void read_dat(
		const uint512_dt*d, hls::stream<uint256_dt> &d_stm,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B){

	#define SHIFT (4-FPPREC)

	ap_uint<8> XBlocks = (M >> SHIFT);
	int total_itr = XBlocks * N * B;
	for (int itr = 0; itr < total_itr; itr++){
		#pragma HLS loop_tripcount min=102400 max=204800 avg=204800
		#pragma HLS PIPELINE II=2
		uint512_dt tmp_d =  d[itr];
		d_stm << tmp_d.range(255,0);
		d_stm << tmp_d.range(511,256);
	}

	#undef SHIFT

}

template <bool FPPREC>
static void read_dat512(
		const uint512_dt*d, hls::stream<uint512_dt> &d_stm,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B){

	#define SHIFT (4-FPPREC)

	ap_uint<8> XBlocks = (M >> SHIFT);
	int total_itr = XBlocks * N * B;
	for (int itr = 0; itr < total_itr; itr++){
		#pragma HLS loop_tripcount min=102400 max=204800 avg=204800
		#pragma HLS PIPELINE II=1
		uint512_dt tmp_d =  d[itr];
		d_stm << tmp_d;
	}

	#undef SHIFT

}


static void FIFO_256_512(hls::stream<uint256_dt> &stm_in, 
	hls::stream<uint512_dt> &stm_out, int total_512){
	for(int i = 0; i < total_512; i++){
		#pragma HLS PIPELINE II=2
		#pragma HLS loop_tripcount min=102400 max=204800 avg=204800
		uint512_dt tmp_w;
		tmp_w.range(255,0) = stm_in.read();
		tmp_w.range(511,256) = stm_in.read();
		stm_out << tmp_w;
	}
}

static void FIFO_512_256(hls::stream<uint512_dt> &stm_in, 
	hls::stream<uint256_dt> &stm_out, int total_512){
	for(int i = 0; i < total_512; i++){
		#pragma HLS PIPELINE II=2
		#pragma HLS loop_tripcount min=102400 max=204800 avg=204800
		uint512_dt tmp_w = stm_in.read();
		stm_out << tmp_w.range(255,0);
		stm_out << tmp_w.range(511,256);
	}
}

static void HBM_fifo(uint512_dt*HBM_buffer1, uint512_dt*HBM_buffer2, 
	hls::stream<uint512_dt> &stm_in, hls::stream<uint512_dt> &stm_out,
		int count_w, int count_b, int count_r){

	int iter = ((count_b + 2* count_w)) >> 1;
	int count_w_N = (count_w >> 1);
	int count_b_N = (count_b >> 1);

	for(int i = 0; i < iter; i++){
		#pragma HLS dependence  variable=HBM_buffer1 WAR distance=1000 true
		#pragma HLS dependence  variable=HBM_buffer2 WAR distance=1000 true
		#pragma HLS loop_tripcount min=60544 max=200000 avg=200000
		#pragma HLS PIPELINE II=2
		uint512_dt tmp_r1, tmp_r2;
		uint512_dt tmp_w1, tmp_w2;

		tmp_r1 =  HBM_buffer1[i];
		tmp_r2 =  HBM_buffer2[i];

		if(i >= count_w_N){
			stm_out << tmp_r1;
			stm_out << tmp_r2;
		}


		if(i < count_w_N + count_b_N){
			tmp_w1 = stm_in.read();
			tmp_w2 = stm_in.read();
		}

		HBM_buffer1[i + count_w_N] = tmp_w1;
		HBM_buffer2[i + count_w_N] = tmp_w2;
	}
}


template <bool FPPREC, int DMAX>
static void interleaved_row_block8(hls::stream<uint256_dt> &stm_in, 
	hls::stream<uint256_dt> &stm_out,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B, bool interleave){

	#define SHIFT (3-FPPREC)
	#define VFACTOR ((1 << SHIFT))
	#define DSIZE 	(256/VFACTOR)
	#define ADJUST (VFACTOR-1)

	ap_uint<12> TileX, TileY;
	ap_uint<24> NTiles;
	ap_uint<8> XBlocks = (M >> SHIFT);
	unsigned int offset;
	const int N_CU = VFACTOR;
	switch(interleave){
		case 1: {TileX=XBlocks; TileY=N_CU; NTiles = (B*N+ADJUST)>>SHIFT; break;}
		case 0: {TileX=8; TileY=N; NTiles = (((XBlocks*B+ADJUST)>>SHIFT)); break;}
		default: {TileX=XBlocks; TileY=N_CU; NTiles = (B*N+ADJUST)>>SHIFT; break;}
	}


	uint256_dt tmp_M[DMAX*N_CU*2];
	#pragma HLS RESOURCE variable=tmp_M core=XPM_MEMORY latency=2

	ap_uint<24> NTilesp1 = register_it<int>(NTiles+1);
	ap_uint<24> id = 0;
	ap_uint<12> jd =0, kd = 0;
	int total_itr = NTilesp1*TileX*TileY;
	for(int itr= 0; itr < total_itr; itr++){
		#pragma HLS PIPELINE II=1
		#pragma HLS loop_tripcount min=1638400 max=2000000 avg=2000000

		ap_uint<24> i = id;
		ap_uint<12> j = jd, k = kd;

		if(k == TileX -1){
			kd = 0;
		} else {
			kd++;
		}

		if(k == TileX -1 && j == TileY -1){
			jd = 0;
			id++;
		} else if(k == TileX -1){
			jd++;
		}
		unsigned int offsetR = ((i & 1) == 0) ?  DMAX*N_CU : 0;
		unsigned int offsetW = ((i & 1) == 0) ?  0 : DMAX*N_CU;

		bool cmpW = !interleave || (i*TileY + j < B*N);
		int indW = k*TileY+j + offsetW;
		uint256_dt tmpW = 0;
		if(cmpW && i < NTiles){
			tmpW = stm_in.read();
		}
		tmp_M[indW] = tmpW;

		int indR = j*TileX + k + offsetR;
		uint256_dt tmpR = tmp_M[indR];
		if(i > 0){
			stm_out << tmpR;
		}
	}
	#undef SHIFT
	#undef VFACTOR
	#undef DSIZE
	#undef ADJUST

}


template <bool FPPREC, int DMAX>
static void row2col(hls::stream<uint256_dt> &in, hls::stream<uint256_dt> &out,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B){


	#define SHIFT (3-FPPREC)
	#define VFACTOR ((1 << SHIFT))
	#define DSIZE 	(256/VFACTOR)
	#define ADJUST (VFACTOR-1)


	ap_uint<12> TileX, TileY;
	ap_uint<20> NTiles;
	ap_uint<8> XBlocks = (M >> SHIFT);
	unsigned int offset;

	TileX = XBlocks;
	TileY = N;
	NTiles = B;

	uint256_dt tmp_M[DMAX*DMAX/VFACTOR*2];
	#pragma HLS RESOURCE variable=tmp_M core=XPM_MEMORY uram latency=2
	uint256_dt tmp;

	ap_uint<18> NTilesp1 = register_it<int>(B+1);
	ap_uint<18> id = 0;
	ap_uint<12> jd =0, kd = 0;
	int total_itr = NTilesp1*TileX*TileY;
	for(int itr= 0; itr < total_itr; itr++){
		#pragma HLS PIPELINE II=1
		#pragma HLS loop_tripcount min=1638400 max=2000000 avg=2000000

		ap_uint<18> i = id;
		ap_uint<12> j = jd, k = kd;

		if(k == TileX -1){
			kd = 0;
		} else {
			kd++;
		}

		if(k == TileX -1 && j == TileY -1){
			jd = 0;
			id++;
		} else if(k == TileX -1){
			jd++;
		}
		int indW = k*TileY+j;
		int indR = j*TileX + k;
		unsigned int offsetR = ((i & 1) == 0) ?  DMAX*DMAX/VFACTOR : 0;
		unsigned int offsetW = ((i & 1) == 0) ?  0 : DMAX*DMAX/VFACTOR;
		if(i < B){
			tmp = in.read();
		}



		tmp_M[indW+offsetW] = tmp;
		uint256_dt tmp_R = tmp_M[indR+offsetR];
		if(i > 0){
			out << tmp_R;
		}
	}

	#undef SHIFT
	#undef VFACTOR
	#undef DSIZE
	#undef ADJUST
}

template <bool FPPREC, int DMAX>
static void undo_interleaved_row_block8(hls::stream<uint256_dt> &stm_in, hls::stream<uint256_dt> &stm_out,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B, bool undo_interleave){


	#define SHIFT (3-FPPREC)
	#define VFACTOR ((1 << SHIFT))
	#define DSIZE 	(256/VFACTOR)
	#define ADJUST (VFACTOR-1)

	ap_uint<12> TileX, TileY;
	ap_uint<24> NTiles;
	ap_uint<8> XBlocks = (M >> SHIFT);
	unsigned int offset;
	const int N_CU = VFACTOR;
	switch(undo_interleave){
		case 1: {TileX=XBlocks; TileY=N_CU; NTiles = (B*N+ADJUST)>>SHIFT; break;}
		case 0: {TileX=8; TileY=N; NTiles = (((XBlocks*B+ADJUST)>>SHIFT)); break;}
		default: {TileX=XBlocks; TileY=N_CU; NTiles = (B*N+ADJUST)>>SHIFT; break;}
	}
	uint256_dt tmp_M[DMAX*N_CU*2];
	#pragma HLS RESOURCE variable=tmp_M core=XPM_MEMORY latency=2
	ap_uint<24> NTilesp1 = register_it<int>(NTiles+1);

	ap_uint<24> id = 0;
	ap_uint<12> jd =0, kd = 0;
	int total_itr = NTilesp1*TileX*TileY;
	for(int itr= 0; itr < total_itr; itr++){
		#pragma HLS PIPELINE II=1
		#pragma HLS loop_tripcount min=1638400 max=2000000 avg=2000000

		ap_uint<24> i = id;
		ap_uint<12> j = jd, k = kd;

		if(k == TileX -1){
			kd = 0;
		} else {
			kd++;
		}

		if(k == TileX -1 && j == TileY -1){
			jd = 0;
			id++;
		} else if(k == TileX -1){
			jd++;
		}
		unsigned int offsetR = ((i & 1) == 0) ?  DMAX*N_CU : 0;
		unsigned int offsetW = ((i & 1) == 0) ?  0 : DMAX*N_CU;

		bool cmpW = !undo_interleave || (i*TileY + j < B*N);
		int indW = j*TileX + k + offsetW;
		uint256_dt tmpW = 0;
		if(cmpW && i < NTiles){
			tmpW = stm_in.read();
		}
		tmp_M[indW] = tmpW;

		int indR = k*TileY+j + offsetR;
		uint256_dt tmpR = tmp_M[indR];
		if(i > 0){
			stm_out << tmpR;
		}
	}

	#undef SHIFT
	#undef VFACTOR
	#undef DSIZE
	#undef ADJUST
}


template <bool FPPREC, int DMAX>
static void col2row(hls::stream<uint256_dt> &in, hls::stream<uint256_dt> &out0,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B){



	#define SHIFT (3-FPPREC)
	#define VFACTOR ((1 << SHIFT))
	#define DSIZE 	(256/VFACTOR)
	#define ADJUST (VFACTOR-1)

	ap_uint<12> TileX, TileY;
	ap_uint<18> NTiles;
	ap_uint<8> XBlocks = (M >> SHIFT);
	unsigned int offset;

	TileX = XBlocks;
	TileY = N;
	NTiles = B;


	uint256_dt tmp_M[DMAX*DMAX/VFACTOR*2];
	#pragma HLS RESOURCE variable=tmp_M core=XPM_MEMORY uram latency=2
	uint256_dt tmp;

	ap_uint<18> NTilesp1 = register_it<int>(B+1);
	ap_uint<18> id = 0;
	ap_uint<12> jd =0, kd = 0;
	int total_itr = NTilesp1*TileX*TileY;
	for(int itr= 0; itr < total_itr; itr++){
		#pragma HLS PIPELINE II=1
		#pragma HLS loop_tripcount min=1638400 max=2000000 avg=2000000

		ap_uint<18> i = id;
		ap_uint<12> j = jd, k = kd;

		if(k == TileX -1){
			kd = 0;
		} else {
			kd++;
		}

		if(k == TileX -1 && j == TileY -1){
			jd = 0;
			id++;
		} else if(k == TileX -1){
			jd++;
		}

//		bool cmp = (i*TileY + j < B*N);

		int indW = j*TileX + k;
		int indR = k*TileY+j;
		unsigned int offsetR = ((i & 1) == 0) ?  DMAX*DMAX/VFACTOR : 0;
		unsigned int offsetW = ((i & 1) == 0) ?  0 : DMAX*DMAX/VFACTOR;

		if(i < B){
			tmp = in.read();
		}

		tmp_M[indW+offsetW] = tmp;
		uint256_dt tmp_R = tmp_M[indR+offsetR];
		if(i > 0){
			out0 << tmp_R;
		}
	}

	#undef SHIFT
	#undef VFACTOR
	#undef DSIZE
	#undef ADJUST
}


template <bool FPPREC, class DType>
static void stream_8x8transpose(hls::stream<uint256_dt> &in, hls::stream<uint256_dt> &out0,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B, bool transpose){

	#define SHIFT (3-FPPREC)
	#define VFACTOR ((1 << SHIFT))
	#define DSIZE 	(256/VFACTOR)
	#define ADJUST (VFACTOR-1)

	const int l_interval = VFACTOR;

	ap_uint<12> TileX, TileY;
	ap_uint<32> NTiles;
	ap_uint<8> XBlocks = (M >> SHIFT);
	const int N_CU = FPPREC? 4 :8;

	switch(transpose){
		case 0: {TileX=XBlocks; TileY=N_CU; NTiles = ((B*N+ADJUST)>>SHIFT)*XBlocks; break;}
		case 1: {TileX=8; TileY=N; NTiles = (((XBlocks*B+ADJUST)>>SHIFT))*N; break;}
		default: {TileX=XBlocks; TileY=N_CU; NTiles = ((B*N+ADJUST)>>SHIFT)*XBlocks; break;}
	}


	loop_read: for(int itr=0; itr < NTiles; itr++){
		#pragma HLS loop_tripcount min=25600 max=204800 avg=204800
		#pragma HLS PIPELINE II=l_interval
		uint256_dt tmp[VFACTOR], outR;
		for(int i = 0; i < VFACTOR; i++){
			tmp[i] = in.read();

		}

		if(transpose){
			for(int i = 0; i < VFACTOR; i++){
				outR.range(DSIZE*1-1, DSIZE*0) = tmp[0].range(DSIZE*(i+1)-1,DSIZE*i);
				outR.range(DSIZE*2-1, DSIZE*1) = tmp[1].range(DSIZE*(i+1)-1,DSIZE*i);
				outR.range(DSIZE*3-1, DSIZE*2) = tmp[2].range(DSIZE*(i+1)-1,DSIZE*i);
				outR.range(DSIZE*4-1, DSIZE*3) = tmp[3].range(DSIZE*(i+1)-1,DSIZE*i);
				if(!FPPREC){
					outR.range(DSIZE*5-1, DSIZE*4) = tmp[4].range(DSIZE*(i+1)-1,DSIZE*i);
					outR.range(DSIZE*6-1, DSIZE*5) = tmp[5].range(DSIZE*(i+1)-1,DSIZE*i);
					outR.range(DSIZE*7-1, DSIZE*6) = tmp[6].range(DSIZE*(i+1)-1,DSIZE*i);
					outR.range(DSIZE*8-1, DSIZE*7) = tmp[7].range(DSIZE*(i+1)-1,DSIZE*i);
				}
				out0 << outR;
			}

		} else {
			for(int i = 0; i < VFACTOR; i++){
					out0 << tmp[i];
			}
		}

	}

	#undef SHIFT
	#undef VFACTOR
	#undef DSIZE
	#undef ADJUST
}

template <bool FPPREC>
static void write_dat(uint512_dt* u, hls::stream<uint256_dt> &u_stm,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B){

	#define SHIFT (4-FPPREC)

	ap_uint<8> XBlocks = (M >> SHIFT);
	int toltal_itr = XBlocks * N * B;
	for(int itr= 0; itr < toltal_itr; itr++){
		#pragma HLS PIPELINE II=2
		#pragma HLS loop_tripcount min=102400 max=204800 avg=204800
		uint512_dt tmp;
		tmp.range(255,0) = u_stm.read();
		tmp.range(511,256) = u_stm.read();
		u[itr] = tmp;;
	}

	#undef SHIFT
}

template <bool FPPREC>
static void write_dat512(uint512_dt* u, hls::stream<uint512_dt> &u_stm,
		ap_uint<12> M, ap_uint<12> N, ap_uint<14> B){
	#define SHIFT (4-FPPREC)
	ap_uint<8> XBlocks = (M >> SHIFT);
	int toltal_itr = XBlocks * N * B;
	for(int itr= 0; itr < toltal_itr; itr++){
		#pragma HLS PIPELINE II=1
		#pragma HLS loop_tripcount min=102400 max=204800 avg=204800
		uint512_dt tmp;
		tmp = u_stm.read();
		u[itr] = tmp;
	}
	#undef SHIFT
}




template<bool FPPREC>
static void read_Tile512(
		const uint512_dt*d, hls::stream<uint512_dt> &d_stm,
		ap_uint<12> M, ap_uint<12> N, ap_uint<8> B, unsigned char dirXYZ){

	#define SHIFT (4-FPPREC)

	ap_uint<12> TileX;
	ap_uint<22> TileY;
	ap_uint<8> NTiles;
	ap_uint<8> XBlocks = (M >> SHIFT);
	unsigned int offset;
	switch(dirXYZ){
		case 0: {TileX=XBlocks; TileY=B*N; NTiles = 1; break;}
		case 1: {TileX=4; TileY=B*N; NTiles = (((XBlocks)>>2)); break;}
		default: {TileX=XBlocks; TileY=N; NTiles = B; break;}
	}

	ap_uint<8> id = 0;
	ap_uint<22> jd =0;
	int total_itr = NTiles*TileY;
	for(int itr= 0; itr < total_itr; itr++){
		#pragma HLS loop_tripcount min=1638400 max=2000000 avg=2000000
		ap_uint<8> i = id;
		ap_uint<22> j = jd;

		if(j == TileY -1){
			jd = 0;
			id++;
		} else {
			jd++;
		}

		int offset = j * XBlocks + i* TileX;
		for(ap_uint<12>  k = 0; k < TileX; k++){
			#pragma HLS loop_tripcount min=100 max=255 avg=255
			#pragma HLS PIPELINE II=1
			uint512_dt tmp_d =  d[offset+k];
			d_stm << tmp_d;
		}
	}

	#undef SHIFT

}


// TDMA Modules
template<bool FPPREC>
static void read_plane(const uint512_dt*d, hls::stream<uint256_dt> &d_stm,
		ap_uint<9> M, ap_uint<9> N, ap_uint<9> L, ap_uint<10> B, unsigned char dirXYZ){


	#define SHIFT (4-FPPREC)

	ap_uint<9> d0,d1,d2;
	ap_uint<24> off_d0, off_d1, off_d2;
	switch(dirXYZ){
		case 0 : {d0 = (M>>SHIFT); d1 = N; d2 = L; off_d0 = 1; off_d1 = (M>>SHIFT); off_d2 = N*(M>>SHIFT); break;}
		case 1 : {d0 = (M>>SHIFT); d1 = N; d2 = L; off_d0 = 1; off_d1 = (M>>SHIFT); off_d2 = N*(M>>SHIFT); break;}
		case 2 : {d0 = (M>>SHIFT); d1 = L; d2 = N; off_d0 = 1; off_d1 = N*(M>>SHIFT); off_d2 = (M>>SHIFT); break;}
		default :{d0 = (M>>SHIFT); d1 = N; d2 = L; off_d0 = 1; off_d1 = (M>>SHIFT); off_d2 = N*(M>>SHIFT); break;}
	}



	ap_uint<10> batd = 0;
	ap_uint<10> id =0;
	ap_uint<10> jd = 0;
	int total_itr = B*d2*d1;
	for(int itr= 0; itr < total_itr; itr++){
		#pragma HLS PIPELINE II=1
		#pragma HLS loop_tripcount min=1638400 max=2000000 avg=2000000

		ap_uint<10> bat = batd;
		ap_uint<10> i = id;
		ap_uint<10> j = jd;


		if(j == d1 -1){
			jd = 0;
		} else {
			jd++;
		}

		if(j == d1 -1 && i == d2 -1){
			id = 0;
			batd++;
		} else if(j == d1 -1){
			id++;
		}
		int ind = bat*(M>>SHIFT)*N*L + j*off_d1 + i *off_d2;
		for(ap_uint<9> k = 0; k < d0; k++){
			#pragma HLS loop_tripcount min=2 max=16 avg=16
			#pragma HLS PIPELINE II=2
			uint512_dt tmp_d =  d[ind+k];

			d_stm << tmp_d.range(255,0);
			d_stm << tmp_d.range(511,256);
		}

	}

	#undef SHIFT

}

template<bool FPPREC>
static void read_plane512(const uint512_dt*d, hls::stream<uint512_dt> &d_stm,
		ap_uint<9> M, ap_uint<9> N, ap_uint<9> L, ap_uint<10> B, unsigned char dirXYZ){


	#define SHIFT (4-FPPREC)

	ap_uint<9> d0,d1,d2;
	ap_uint<24> off_d0, off_d1, off_d2;
	switch(dirXYZ){
		case 0 : {d0 = (M>>SHIFT); d1 = N; d2 = L; off_d0 = 1; off_d1 = (M>>SHIFT); off_d2 = N*(M>>SHIFT); break;}
		case 1 : {d0 = (M>>SHIFT); d1 = N; d2 = L; off_d0 = 1; off_d1 = (M>>SHIFT); off_d2 = N*(M>>SHIFT); break;}
		case 2 : {d0 = (M>>SHIFT); d1 = L; d2 = N; off_d0 = 1; off_d1 = N*(M>>SHIFT); off_d2 = (M>>SHIFT); break;}
		default :{d0 = (M>>SHIFT); d1 = N; d2 = L; off_d0 = 1; off_d1 = (M>>SHIFT); off_d2 = N*(M>>SHIFT); break;}
	}

	ap_uint<10> batd = 0;
	ap_uint<10> id =0;
	ap_uint<10> jd = 0;
	int total_itr = B*d2*d1;
	for(int itr= 0; itr < total_itr; itr++){
		#pragma HLS PIPELINE II=1
		#pragma HLS loop_tripcount min=1638400 max=2000000 avg=2000000

		ap_uint<10> bat = batd;
		ap_uint<10> i = id;
		ap_uint<10> j = jd;


		if(j == d1 -1){
			jd = 0;
		} else {
			jd++;
		}

		if(j == d1 -1 && i == d2 -1){
			id = 0;
			batd++;
		} else if(j == d1 -1){
			id++;
		}
		int ind = bat*(M>>SHIFT)*N*L + j*off_d1 + i *off_d2;
		for(ap_uint<9> k = 0; k < d0; k++){
			#pragma HLS loop_tripcount min=2 max=16 avg=16
			#pragma HLS PIPELINE II=1
			uint512_dt tmp_d =  d[ind+k];

			d_stm << tmp_d;
		}

	}

	#undef SHIFT

}

template<bool FPPREC>
static void write_plane(uint512_dt* u, hls::stream<uint256_dt> &u_stm,
		ap_uint<9> M, ap_uint<9> N, ap_uint<9> L, ap_uint<10> B, unsigned char dirXYZ){


	#define SHIFT (4-FPPREC)

	ap_uint<9> d0,d1,d2;
	ap_uint<24> off_d0, off_d1, off_d2;
	switch(dirXYZ){
		case 0 : {d0 = (M>>SHIFT); d1 = N; d2 = L; off_d0 = 1; off_d1 = (M>>SHIFT); off_d2 = N*(M>>SHIFT); break;}
		case 1 : {d0 = (M>>SHIFT); d1 = N; d2 = L; off_d0 = 1; off_d1 = (M>>SHIFT); off_d2 = N*(M>>SHIFT); break;}
		case 2 : {d0 = (M>>SHIFT); d1 = L; d2 = N; off_d0 = 1; off_d1 = N*(M>>SHIFT); off_d2 = (M>>SHIFT); break;}
		default :{d0 = (M>>SHIFT); d1 = N; d2 = L; off_d0 = 1; off_d1 = (M>>SHIFT); off_d2 = N*(M>>SHIFT); break;}
	}

	ap_uint<10> batd = 0;
	ap_uint<10> id =0;
	ap_uint<10> jd = 0;
	int total_itr = B*d2*d1;
	for(int itr= 0; itr < total_itr; itr++){
		#pragma HLS PIPELINE II=1
		#pragma HLS loop_tripcount min=1638400 max=2000000 avg=2000000

		ap_uint<10> bat = batd;
		ap_uint<10> i = id;
		ap_uint<10> j = jd;


		if(j == d1 -1){
			jd = 0;
		} else {
			jd++;
		}

		if(j == d1 -1 && i == d2 -1){
			id = 0;
			batd++;
		} else if(j == d1 -1){
			id++;
		}
		int ind = bat*(M>>SHIFT)*N*L + j*off_d1 + i *off_d2;
		for(ap_uint<9> k = 0; k < d0; k++){
			#pragma HLS loop_tripcount min=2 max=16 avg=16
			#pragma HLS PIPELINE II=2
			uint512_dt tmp;
			tmp.range(255,0) = u_stm.read();
			tmp.range(511,256) = u_stm.read();
			u[ind+k] = tmp;
		}
	}

	#undef SHIFT

}

template<bool FPPREC>
static void write_plane512(uint512_dt* u, hls::stream<uint512_dt> &u_stm,
		ap_uint<9> M, ap_uint<9> N, ap_uint<9> L, ap_uint<10> B, unsigned char dirXYZ){


	#define SHIFT (4-FPPREC)

	ap_uint<9> d0,d1,d2;
	ap_uint<24> off_d0, off_d1, off_d2;
	switch(dirXYZ){
		case 0 : {d0 = (M>>SHIFT); d1 = N; d2 = L; off_d0 = 1; off_d1 = (M>>SHIFT); off_d2 = N*(M>>SHIFT); break;}
		case 1 : {d0 = (M>>SHIFT); d1 = N; d2 = L; off_d0 = 1; off_d1 = (M>>SHIFT); off_d2 = N*(M>>SHIFT); break;}
		case 2 : {d0 = (M>>SHIFT); d1 = L; d2 = N; off_d0 = 1; off_d1 = N*(M>>SHIFT); off_d2 = (M>>SHIFT); break;}
		default :{d0 = (M>>SHIFT); d1 = N; d2 = L; off_d0 = 1; off_d1 = (M>>SHIFT); off_d2 = N*(M>>SHIFT); break;}
	}

	ap_uint<10> batd = 0;
	ap_uint<10> id =0;
	ap_uint<10> jd = 0;
	int total_itr = B*d2*d1;
	for(int itr= 0; itr < total_itr; itr++){
		#pragma HLS PIPELINE II=1
		#pragma HLS loop_tripcount min=1638400 max=2000000 avg=2000000

		ap_uint<10> bat = batd;
		ap_uint<10> i = id;
		ap_uint<10> j = jd;


		if(j == d1 -1){
			jd = 0;
		} else {
			jd++;
		}

		if(j == d1 -1 && i == d2 -1){
			id = 0;
			batd++;
		} else if(j == d1 -1){
			id++;
		}
		int ind = bat*(M>>SHIFT)*N*L + j*off_d1 + i *off_d2;
		for(ap_uint<9> k = 0; k < d0; k++){
			#pragma HLS loop_tripcount min=2 max=16 avg=16
			#pragma HLS PIPELINE II=2
			uint512_dt tmp;
			tmp = u_stm.read();
			u[ind+k] = tmp;
		}
	}

	#undef SHIFT

}

template<bool FPPREC>
static void write_Tile512(uint512_dt* u, hls::stream<uint512_dt> &u_stm,
		ap_uint<12> M, ap_uint<12> N, ap_uint<12> B, unsigned char dirXYZ){


	#define SHIFT (4-FPPREC)

	ap_uint<12> TileX;
	ap_uint<22> TileY;
	ap_uint<8> NTiles;
	ap_uint<8> XBlocks = (M >> SHIFT);
	unsigned int offset;
	switch(dirXYZ){
		case 0: {TileX=XBlocks; TileY=B*N; NTiles = 1; break;}
		case 1: {TileX=4; TileY=B*N; NTiles = (((XBlocks)>>2)); break;}
		default: {TileX=XBlocks; TileY=N; NTiles = B; break;}
	}

	ap_uint<8> id = 0;
	ap_uint<22> jd =0;
	int total_itr = NTiles*TileY;
	for(int itr= 0; itr < total_itr; itr++){
		#pragma HLS loop_tripcount min=1638400 max=2000000 avg=2000000
		ap_uint<8> i = id;
		ap_uint<22> j = jd;

		if(j == TileY -1){
			jd = 0;
			id++;
		} else {
			jd++;
		}

		int offset = j * XBlocks + i* TileX; //(j*((XBlocks*B+3)>>2)<<2);
		for(ap_uint<12> k = 0; k < TileX; k++){
			#pragma HLS loop_tripcount min=100 max=255 avg=255
			#pragma HLS PIPELINE II=1
			uint512_dt tmp;
			tmp = u_stm.read();
			u[offset+k] = tmp;;
		}
	}

	#undef SHIFT

}


template<bool FPPREC, class DType>
static void vec_to_Dtype(hls::stream<uint256_dt> &stm_vec_in, hls::stream<DType> &stm_scl_out, int size){

	#define SHIFT (3-FPPREC)
	#define VFACTOR ((1 << SHIFT))
	#define DSIZE 	(256/VFACTOR)

	for(int itr = 0; itr < size; itr++){
		#pragma HLS PIPELINE II=8
		uint256_dt tmp = stm_vec_in.read();
		for(int v = 0; v < VFACTOR; v++){
			float val = uint2FP_ript<FPPREC, DType>(tmp.range(DSIZE*(v+1)-1,DSIZE*v));
			stm_scl_out << val;
		}
	}

	#undef SHIFT
	#undef VFACTOR
	#undef DSIZE
}


template<bool FPPREC, class DType>
static void Dtype_to_vec(hls::stream<DType> &stm_scl_in, hls::stream<uint256_dt> &stm_vec_out, int size){

	#define SHIFT (3-FPPREC)
	#define VFACTOR ((1 << SHIFT))
	#define DSIZE 	(256/VFACTOR)

	for(int itr = 0; itr < size; itr++){
		#pragma HLS PIPELINE II=8
		uint256_dt tmp;
		for(int v = 0; v < VFACTOR; v++){
			tmp.range(DSIZE*(v+1)-1,DSIZE*v) = FP2uint_ript(stm_scl_in.read());
		}
		stm_vec_out << tmp;
	}

	#undef SHIFT
	#undef VFACTOR
	#undef DSIZE
}



template <int MEM_SIZE>
static void URAM_buffer(hls::stream<uint256_dt> &in_stm_1, hls::stream<uint256_dt> &out_stm_1,
		int total_data, ap_uint<20> delay){

	uint256_dt mem[MEM_SIZE];
	#pragma HLS RESOURCE variable=mem core=XPM_MEMORY uram latency=2
	int total_itr = total_data + delay;
	ap_uint<20> count = 0;

	for(int i = 0; i < total_itr; i++){
		#pragma HLS loop_tripcount min=500000 max=2000000 avg=2000000
		#pragma HLS PIPELINE II=1

		if(i >= delay){
			out_stm_1 << mem[count];
		}

		if(i < total_data){
			mem[count] = in_stm_1.read();
		}

		if(count >= delay -1){
			count = 0;
		} else{
			count++;
		}
	}
}



#endif


