mkByCt = {
  [0] = 0,
  [1] = 40,
  [2] = 140,
  [3] = 0,
  [4] = 0,
}

function ondraw()
  if who ~= self or rinshan then
    return
  end
  
  local closed = game:gethand(self):closed()
  for _, t in ipairs(T34.all) do
    mount:lighta(t, mkByCt[closed:ct(t)])
  end
end
