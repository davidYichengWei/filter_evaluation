-- Insert into t1
INSERT INTO t1 (col1, col2, col3, col4, col5)
SELECT 100, 100, 100, 100, 100
FROM generate_series(1, 10000000);

-- Insert into t2
INSERT INTO t2 (col1, col2, col3, col4, col5, col6, col7, col8, col9, col10)
SELECT 100, 100, 100, 100, 100, 100, 100, 100, 100, 100
FROM generate_series(1, 10000000);

-- Insert into t3
INSERT INTO t3 (col1, col2, col3, col4, col5, col6, col7, col8, col9, col10, col11, col12, col13, col14, col15, col16, col17, col18, col19, col20)
SELECT 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100
FROM generate_series(1, 10000000);
