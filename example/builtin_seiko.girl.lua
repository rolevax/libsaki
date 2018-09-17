function checkinit()
  if who ~= self or iter > 100 then
    return true
  end

  -- no triplet
  for _, t in ipairs(T34.all) do
    if init:closed():ct(t) >= 3 then
      return false
    end
  end

  local yaopairct = 0
  local yaos = {
    "1m", "9m", "1p", "9p", "1s", "9s",
    "1f", "2f", "3f", "4f", "1y", "2y", "3y"
  }
  
  for _, str in ipairs(yaos) do
    local t = T34.new(str)
    if init:closed():ct(t) == 2 then
      yaopairct = yaopairct + 1
    end
  end
  
  return yaopairct >= 2
end

function ondraw()
  if who ~= self or rinshan then
    return
  end

  local hand = game:gethand(self)
  local closed = hand:closed()
  local barks = hand:barks()

  local function isfish(meld)
    local t = meld:type()
    return t == "pon" or t == "daiminkan" or t == "kakan"
  end

  local fishct = 0
  for _, m in ipairs(barks) do
    if isfish(m) then
      fishct = fishct + 1
    end
  end

  if fishct >= 3 then
    accelerate(mount, hand, game:getriver(self), 500)
  elseif not hand:ready() then
    local needpair = 4 - fishct
    for _, t in ipairs(T34.all) do
      if closed:ct(t) == 2 then
        needpair = needpair - 1
        mount:lighta(t, -400);
      end
    end

    if needpair > 0 then
      local nextpairs = {
        "1m", "2m", "8m", "9m",
        "1p", "2p", "8p", "9p",
        "1s", "2s", "8s", "9s",
        "1f", "2f", "3f", "4f",
        "1y", "2y", "3y"
      }

      for _, str in ipairs(nextpairs) do
        local t = T34.new(str)
        if closed:ct(t) == 1 then
          mount:lighta(t, 400)
        end
      end
    end
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
