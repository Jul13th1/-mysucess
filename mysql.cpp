#include <mysql/mysql.h>
#include <iostream>
#include <string>

using namespace std;

int main() 
{
    MYSQL *conn;
    MYSQL_RES *res; //声明一个指向查询结果的指针 res，用于存储查询返回的数据。
    MYSQL_ROW row;  //声明一个行指针 row，用于存储查询结果中的一行数据。

    // 初始化 MySQL 连接
    conn = mysql_init(NULL);
    if (conn == NULL) 
    {
        cerr << "数据库初始化失败";
        return EXIT_FAILURE;
    }

    // 连接到 MySQL 数据库
    if (mysql_real_connect(conn, "127.0.0.1", "root", "123456", "testdb", 3306, NULL, 0) == NULL) 
    {
        cerr << "数据库连接失败\n";
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // 启用事务
    if (mysql_query(conn, "START TRANSACTION;")) 
    {
        cerr << "启动事务日志失败: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // 执行一些 SQL 操作，操作将被记录到 binlog 中
    if (mysql_query(conn, "INSERT INTO students (id,name, age) VALUES (1002,'Alice', 30);")) 
    {
        cerr << "插入失败: " << mysql_error(conn) << endl;
        //如果插入失败就回滚
        mysql_query(conn, "ROLLBACK;");
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // 提交事务
    if (mysql_query(conn, "COMMIT;")) 
    {
        cerr << "COMMIT failed: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // 查询结果以验证数据插入
    if (mysql_query(conn, "SELECT id,name, age FROM students;")) 
    {
        cerr << "查询失败: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    res = mysql_store_result(conn);
    if (res == NULL) 
    {
        cerr << "mysql_store_result() failed: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // 输出查询结果
    while ((row = mysql_fetch_row(res)) != NULL) {
        cout << "id: " << row[0] << ", name: " << row[1] << ",age: " << row[2] << endl;
    }

    // 清理并关闭连接
    mysql_free_result(res);
    mysql_close(conn);

    return EXIT_SUCCESS;
}