module Hw11 where
import Control.Monad
import Data.Maybe
import Control.Monad.Error
import Data.Char(isHexDigit, digitToInt, isNumber, isPunctuation)
import Control.Monad.Trans.Maybe

--(2 балла) Какие из законов класса типов MonadPlus выполняются для списка? типа Maybe? Приведите доказательство или опровергающий пример
-- Решение
-- 1. Список:
-- left zero - выполняется
-- ([] >>= \x->[x]) == [] 
-- left distribution - выполняется:
--("aa" ++ "bcd" >>= \x->[x]) == (("aa" >>= \x->[x]) ++ ("bcd" >>= \x->[x])) --left distribution ok
-- left catch flow - не выполняется!
-- ((return "abcd") ++ (return "cdef")) == (return "abcd") -- не выполняется

-- 2. Maybe:
-- left zero: выполняется
-- (Nothing >>= id) == Nothing 
-- left distribution:
mplus' :: Maybe a -> Maybe a -> Maybe a
mplus' Nothing b = b
mplus' a _ = a 
-- для проверки добавим функцию:
-- подберем такую k, чтобы закон не выполнялся:
k:: Eq a=> Maybe a -> a -> Maybe a
k = \a x -> if x == (fromJust a) then Nothing else (Just x)
leftDistr :: Eq a => Maybe a -> Maybe a -> Bool
leftDistr = \a b->(a `mplus'` b >>= (k a)) == ((a >>= (k a)) `mplus'` (b >>= (k a)))
-- leftDistr (Nothing) (Just "a"), leftDistr (Just "a") (Nothing) и leftDistr Nothing Nothings 
-- возвращают True (т.е. контрпример пока не нашли), зато 
-- leftDistr (Just "a") (Just "b") -- возвращает False,
-- то есть нашли контрпример - закон не выполняется.
--
-- left catch flow:
--(return "a") `mplus'` (Just "b") == return "a" -- выполняется,
-- т.к. return = Just, и с учетом определения  `mplus'`, которое привели выше.

--(2 балла) Разработайте функции
--(вспомогательный кусок)
data ParseError = Err {location :: Int, reason :: String} deriving Show
instance Error ParseError where 
  noMsg = Err 0 "Parse Error"
  strMsg s = Err 0 s

type ParseMonad = Either ParseError
----
--функции:
parseHex :: String -> ParseMonad Integer
parseHex s = helper s 1 0
  where
    helper :: String -> Int -> Integer -> ParseMonad Integer
    helper "" pos res = Right res
    helper (x:xs) pos res = if (isHexDigit x) then (helper xs (pos + 1) (res*16 + (fromIntegral $ digitToInt x)))
              else (throwError (Err pos ([x] ++ ": invalid digit")))

printError ::ParseError -> ParseMonad String
printError  err = return ("At pos " ++ (show $ location err) ++ ": " ++ (reason err))

test s = str where
  (Right str) = do
      n <- parseHex s
      return $ show n
    `catchError` printError

--(2 балла) Используя пользовательский тип ошибки и трансформер ErrorT, модифицируйте этот код так, 
-- чтобы он выдавал пользователю сообщение о причине, по которой пароль отвергнут
askPassword :: MaybeT IO ()
askPassword = do
  liftIO $ putStrLn "Insert your new password:"
  value <- msum $ repeat getValidPassword
  liftIO $ putStrLn "Storing in database..."

getValidPassword :: MaybeT IO String
getValidPassword = do
  s <- liftIO getLine
  result <- runErrorT (isValid s)
  case result of 
    (Left err) -> do { 
      liftIO $ putStrLn (reason err);
      guard False ;
      return "ok"}
    (Right res) -> do {return "ok"}
  --guard (isValid s)

isValid :: String -> ErrorT ParseError (MaybeT IO) Bool
isValid s | length s < 8 = throwError (Err 0 "length must be greater or equal 8")
          | not (any isNumber s) = throwError (Err 0 "password must contain at least 1 digit")
          | not (any isPunctuation s) = throwError (Err 0 "password must contain some punctuation chars")
          | otherwise = return True

main = runMaybeT askPassword