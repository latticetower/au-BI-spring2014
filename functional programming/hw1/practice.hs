module My where
  f1 x = (+) x 3
  f2 = \x -> (+) x 3
  sign x | x >0 = 1
         | x == 0 = 0
         | x < 0 = -1
  nor :: Bool -> Bool -> Bool
  nor False False = True
  nor _ _ = False
  
  nand :: Bool -> Bool -> Bool
  nand True True = False
  nand _ _ = True
  
  fib k = helper 1 1 k
    where helper a0 a1 k | k > 1 = helper a1 (a0 + a1) (k - 1)
                         | k == 1 = a1 
                         | k == 0 = a0
                         
  flip :: (a -> b -> c) -> b -> a -> c
  flip f x y = f y x
  
  sq = on (*) (^2)