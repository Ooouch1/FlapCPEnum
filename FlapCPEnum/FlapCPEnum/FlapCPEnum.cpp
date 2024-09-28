// FlapCPEnum.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "AppMain.hpp"
int main(int argc, char *argv[])
{
	
	MPI_Init(&argc, &argv);
	AppMain app;
	
	int ret = app.runMain(argc, argv);

	MPI_Finalize();

	return ret;

}
