實作 K-means 演算法

將所有像素點的 RGB 數值當作是 ( x, y, z ) 立體座標。與其他像素點使用歐幾里得距離公式計算距離。輸入想要分的群數 K 後，依據 K-means 演算法作法進行分群。

輸入檔名為：input.bmp  
為Bmp屬性圖片  

輸出檔案為 output"i".bmp 其中 "i" 為手動輸入希望有多少個分群 ( K值 )。
輸出圖片內容為分群後的結果，每個分群顏色為各群顏色取平均。最後得到使用K種顏色上色的結果。

展示：  
原圖  
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CK-means%E6%BC%94%E7%AE%97%E6%B3%95/show/input.bmp)  

分群各為 1 ~ 10 的輸出動圖：  
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CK-means%E6%BC%94%E7%AE%97%E6%B3%95/show/outputALL.png)  

--

分群為 1 的輸出圖：  
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CK-means%E6%BC%94%E7%AE%97%E6%B3%95/output1.bmp)  

分群為 2 的輸出圖：  
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CK-means%E6%BC%94%E7%AE%97%E6%B3%95/output2.bmp)  

分群為 3 的輸出圖：  
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CK-means%E6%BC%94%E7%AE%97%E6%B3%95/output3.bmp)  

分群為 4 的輸出圖：  
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CK-means%E6%BC%94%E7%AE%97%E6%B3%95/output4.bmp)  

分群為 5 的輸出圖：  
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CK-means%E6%BC%94%E7%AE%97%E6%B3%95/output5.bmp)  

分群為 6 的輸出圖：  
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CK-means%E6%BC%94%E7%AE%97%E6%B3%95/output6.bmp)  

分群為 7 的輸出圖：  
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CK-means%E6%BC%94%E7%AE%97%E6%B3%95/output7.bmp)  

分群為 8 的輸出圖：  
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CK-means%E6%BC%94%E7%AE%97%E6%B3%95/output8.bmp)  

分群為 9 的輸出圖：  
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CK-means%E6%BC%94%E7%AE%97%E6%B3%95/output9.bmp)  

分群為 10 的輸出圖：  
![image](https://github.com/gta45297/resume/blob/main/%E5%AF%A6%E4%BD%9CK-means%E6%BC%94%E7%AE%97%E6%B3%95/output10.bmp)  
