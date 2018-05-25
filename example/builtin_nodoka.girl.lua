function ondraw()
  if who ~= self or rinshan then
    return
  end
  
  local hand = game:gethand(self)
  local step4 = hand:step4()
  local step7 = hand:step7()
  
  local mk = step4 == 1 and 100 or 200
  if step4 == 1 or step7 == 2 or step7 == 1 then
    for _, t in ipairs(hand:effa4()) do
      mount:lighta(t, mk)
    end
  end
end