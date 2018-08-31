function ondraw()
  if who ~= self then
    return
  end

  local guests, hosts = getwinds(game, self)
  local hand = game:gethand(self)
  local step4 = hand:step4()
  local maxguest, maxguestcount = pickmaxguest(hand, guests)

  if step4 <= 1 then
    if hand:ismenzen() then
      if not dyed(hand) then
        for _, t in ipairs(hand:effa()) do
          mount:lighta(t, -50)
        end
      else
        if maxguestcount == 2 then
          mount:lighta(maxguest, 300)
        elseif maxguestcount >= 3 then
          for _, t in ipairs(hand:effa()) do
            mount:lighta(t, 250)
          end
        end
      end
    end
  else
    if maxguestcount == 0 then
      for _, t in ipairs(guests) do
        mount:lighta(t, 500)
      end
    elseif 1 <= maxguestcount and maxguestcount <= 2 then
      mount:lighta(maxguest, 500)
    end

    if maxguestcount == 2 then
      dye(hand:closed(), mount, 100)
    elseif maxguestcount >= 3 then
      dye(hand:closed(), mount, 200)
    end

    for _, h in ipairs(hosts) do
      if (hand:closed():ct(h) >= 2) then
        mount:lighta(h, -50)
      end
    end
  end
end

-- get guest and non-guest winds of this round
function getwinds(game, self)
  local guests = {}
  local hosts = {}
  local sw = game:getselfwind(self)
  local rw = game:getroundwind()
  local winds = { "1f", "2f", "3f", "4f" }
  for _, str in ipairs(winds) do
    local t = T34.new(str)
    if t:isyakuhai(sw, rw) then
      table.insert(hosts, t)
    else
      table.insert(guests, t)
    end
  end

  return guests, hosts
end

-- select the guest-wind tile in 'hand' with the max quantity
function pickmaxguest(hand, guests)
  local maxguest = nil
  local maxguestcount = 0

  for _, g in ipairs(guests) do
    local comax = countguest(hand, g)
    if comax > maxguestcount then
      maxguest = g
      maxguestcount = comax
    end
  end

  return maxguest, maxguestcount
end

-- increase possibilify of hon1tsu
function dye(closed, mount, mk)
  local suits = { "m", "p", "s" }
  local max = 0
  local maxsuit = nil
  for _, suit in ipairs(suits) do
    local comax = closed:ct(suit)
    if comax > max then
      maxsuit = suit
      max = comax
    end
  end

  for i = 1,9 do
    mount:lighta(T34.new(i .. maxsuit), mk)
  end
end

-- check if 'hand' is fully dyed
function dyed(hand)
  local suits = { "m", "p", "s" }
  local suitcount = 0
  for _, suit in ipairs(suits) do
    for i = 1, 9 do
      if hand:ct(T34.new(i .. suit)) > 0 then
        suitcount = suitcount + 1
        if suitcount >= 2 then
          return false
        end

        break
      end
    end
  end

  return true
end

-- count number of 'guest' in 'hand'
-- count closed and ankan only, ignore open melds
function countguest(hand, guest)
  local barks = hand:barks()
  for _, bark in ipairs(barks) do
    if bark:type() == "ankan" and guest == m[0] then
      return 4
    end
  end

  return hand:closed():ct(guest)
end
