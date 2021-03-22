實作 DBSCAN 演算法  

將非白色所有像素點的 ( x, y ) 座標。與其他像素點使用歐幾里得距離公式計算距離。輸入 DBSCAN演算法需要的參數：像素點合併容許最遠距離、每群構成群的最小數量。依據 DBSCAN 演算法進行分群。  

輸入檔名為：input.bmp  
為Bmp屬性圖片  

輸出檔名為： output_D"i"_N"j".bmp 其中 "i" 為輸入的容許最遠距離、 "j" 為輸入的集群最小構成數量  
輸出圖片內容為分群後的結果，每個群使用不同顏色上色。離群座標不標示。  

展示：
原圖
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CDBSCAN%E6%BC%94%E7%AE%97%E6%B3%95/show/input.bmp)

容許最遠距離 i = 1
集群最小構成數量 j = 5
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CDBSCAN%E6%BC%94%E7%AE%97%E6%B3%95/show/output_D1_N5.bmp)

容許最遠距離 i = 2
集群最小構成數量 j = 5
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CDBSCAN%E6%BC%94%E7%AE%97%E6%B3%95/show/output_D2_N5.bmp)

容許最遠距離 i = 3
集群最小構成數量 j = 5
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CDBSCAN%E6%BC%94%E7%AE%97%E6%B3%95/show/output_D3_N5.bmp)
