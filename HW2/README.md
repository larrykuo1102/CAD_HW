# Project 02 M111145046郭睿驛

### 環境需求
1. C++ boost/graph
    如果沒有請輸入
    ```
    make install
    ```
2. Gurobi
    請確認gurobi可以在當前的terminal直接執行
    ```
    gurobi_cl
    ```

### 執行程式
1. 先輸入
    ```
    make build
    ```
    來編譯程式
2. 
    依照題目給的格式 **mlrcs -h/-e BLIF_FILE AND_CONSTRAINT OR_CONSTRAINT NOT_CONSTRAINT** 執行

    (mytest1.blif為自己做的測資)
    ```
    ./mlrcs -h/-e mytest1.blif 2 1 1
    ```

