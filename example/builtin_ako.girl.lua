function ondraw()
  if who ~= self or rinshan then
    return
  end

  local hand = game:gethand(self)
  local seqheads = {} -- beginnings of sequences

  for _, m in ipairs(hand:barks()) do
    if m:type() == "chii" then
      seqheads[#seqheads + 1] = m[0]
    end
  end

  if #seqheads == 1 then
    onedragtwo(mount, hand:closed(), seqheads[1])
  elseif #seqheads == 2 then
    local head1 = seqheads[1]
    local head2 = seqheads[2]
    local samesuit = head1:suit() == head2:suit()
    local absvaldiff = math.abs(head1:val() - head2:val())
    local like3 = not samesuit and head1:val() == head2:val()
    local like1 = samesuit and (absvaldiff == 3 or absvaldiff == 6)

    if like3 then
      local suits = { m = 1, p = 1, s = 1 }
      suits[head1:suit()] = nil
      suits[head2:suit()] = nil
      local lastsuit = pairs(suits)()
      thinfill(mount, hand:closed(), T34.new(lastsuit, head1:val()))
    elseif like1 then
      local v = 12 - (head1:val() + head2:val())
      thinfill(mount, hand:closed(), T34.new(head1:suit(), v))
    else
      local dist3a = sskdist(hand:closed(), head1)
      local dist1a = ittdist(hand:closed(), head1)
      local dist3b = sskdist(hand:closed(), head2)
      local dist1b = ittdist(hand:closed(), head2)
      local mina = math.min(dist3a, dist1a)
      local minb = math.min(dist3b, dist1b)
      if mina < minb then
        onedragtwo(mount, hand:closed(), head1)
      else
        onedragtwo(mount, hand:closed(), head2)
      end
    end
  end

  local drids = mount:getdrids()
  accelerate(mount, hand, game:getriver(self), 15)
  if hand:ctaka5() + drids % hand < 1 then
    for _, t in ipairs(drids) do
      mount:lighta(t:dora(), 80)
    end

    mount:lighta(T37.new("0m"), 30)
    mount:lighta(T37.new("0p"), 30)
    mount:lighta(T37.new("0s"), 30)
  end
end

function accelerate(mount, hand, river, mk)
  local trashes = {}
  for _, t in ipairs(river) do
    trashes[t:id34()] = true
  end

  for _, t in ipairs(hand:effa()) do
    if not trashes[t:id34()] then
      mount:lighta(t, mk)
    end
  end
end

-- calculate distance to sanshoku,
-- when having a chii of [ head, head:next(), head:nnext() ]
-- for example,
-- if head is 3m, we already have chii'ed 345m,
-- then it will return distance to 345p345s
function sskdist(closed, head)
  local suits = { m = 1, p = 1, s = 1 }
  suits[head:suit()] = nil

  local distance = 6
  for suit in pairs(suits) do
      local t = T34.new(suit, head:val())
      local work = math.min(closed:ct(t), 1) + 
                   math.min(closed:ct(t:next()), 1) + 
                   math.min(closed:ct(t:nnext()), 1) 
      distance = distance - work
  end

  return distance
end

-- calculate distance to ittsuu,
-- when having a chii of [ head, head:next(), head:nnext() ]
-- for example,
-- if head is 4m, we already have chii'ed 456m,
-- then it will return distance to 123789m
function ittdist(closed, head)
  local suit = head:suit()
  local val = head:val()
  local sum = 0

  local function sumup(first, last)
    for i = first, last do
      sum = sum + math.min(closed:ct(T34.new(suit, i)), 1)
    end
  end

  if val == 1 then
    sumup(4, 9)
  elseif v == 4 then
    sumup(1, 3)
    sumup(7, 9)
  elseif v == 7 then
    sumup(1, 6)
  else
    sum = -3
  end

  return 6 - sum
end

-- gather sanshoku or ittsuu according to
-- one chii of [ head, head:next(), head:nnext() ]
function onedragtwo(mount, closed, head)
  local dist3 = sskdist(closed, head)
  local dist1 = ittdist(closed, head)
  if dist3 < dist1 then
    if 0 < dist3 and dist3 <= 3 then
      for _, s in ipairs({ "m", "p", "s" }) do
        if s ~= head:suit() then
          thinfill(mount, closed, T34.new(s, head:val()))
        end
      end
    end
  else
    if 0 < dist1 and dist1 <= 3 then
      for _, v in ipairs({ 1, 4, 7 }) do
        if v ~= head:val() then
          thinFill(mount, closed, T34.new(head:suit(), v))
        end
      end
    end
  end
end

-- drag a sequence started from 'head'
function thinfill(mount, closed, head)
  for _, t in ipairs({ head, head:next(), head:nnext() }) do
    if closed:ct(t) == 0 then
      mount:lightA(t, 600)
    end
  end
end
