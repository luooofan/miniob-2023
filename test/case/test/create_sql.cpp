#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
// 左闭右闭区间

//CREATE TABLE big_order_by_0(id int, num int, price float, addr char(10), birthday date);
//CREATE TABLE big_order_by_1(id int, num int, price float, addr char(10), birthday date);
//CREATE TABLE big_order_by_2(id int, num int, price float, addr char(10), birthday date);
//CREATE TABLE big_order_by_3(id int, num int, price float, addr char(10), birthday date);

//select * from big_order_by_0, big_order_by_1, big_order_by_2, big_order_by_3 order by big_order_by_2.num, big_order_by_1.num, big_order_by_1.birthday, big_order_by_3.addr, big_order_by_2.id, big_order_by_1.addr, big_order_by_3.num, big_order_by_0.addr;
int getRand(int min, int max) {
    return ( rand() % (max - min + 1) ) + min ;
}
int main() {
    std::ofstream file("sqls.txt"); // 打开文件，如果文件不存在则创建新文件
    srand(time(0));
    if (file.is_open()) 
    { // 检查文件是否成功打开
        file << "CREATE TABLE big_order_by_0(id int, num int, price float, addr char(10), birthday date);\n";
        file << "CREATE TABLE big_order_by_1(id int, num int, price float, addr char(10), birthday date);\n";
        file << "CREATE TABLE big_order_by_2(id int, num int, price float, addr char(10), birthday date);\n";
        file << "CREATE TABLE big_order_by_3(id int, num int, price float, addr char(10), birthday date);\n";

        for(int j = 0 ; j < 4 ; j++)
        {
            int limit = 100;
            if(j == 3)
                limit = 2;
            for(int i = 0 ; i < limit; ++i)
            {
                std::string sql = "INSERT INTO big_order_by_" +std::to_string(j) + " VALUES ( " + std::to_string(getRand(0,2000)) + " , "
                                +std::to_string(getRand(0,2000)) + ", 78.25, 'O4NXHR', '" +  std::to_string(getRand(1500,2000))+ "-10-01');\n" ;
                file << sql; // 向文件中写入内容
            }
        }

        file << "select * from big_order_by_0, big_order_by_1, big_order_by_2, big_order_by_3 order by big_order_by_2.num, big_order_by_1.num, big_order_by_1.birthday, big_order_by_3.addr, big_order_by_2.id, big_order_by_1.addr, big_order_by_3.num, big_order_by_0.addr;\n";
    }
    else 
    {
        std::cout << "无法打开文件。" << std::endl;
    }

    return 0;
}

 
// int main() {
 

//     for (int i=0; i<10; i++) {
//         int r = getRand(2,20);
//         std::cout << r << std::endl;
//     }
 
//     return 0;
// }
 
