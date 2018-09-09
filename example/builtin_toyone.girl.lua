function ongameevent()
  if event.type == "diced" then
    firstriichi = nil
  elseif event.type == "riichi-established" then
    if not firstriichi then
      firstriichi = event.args.who
    end
  end
end

function ondraw()
  if rinshan then
    return
  end

  senbu(mount, game, who)
  tomobiki(mount, game, who)
end

function senbu(mount, game, who)
  local myhand = game:gethand(self)

  if firstriichi and firstriichi ~= self then
    local catch = who == self and myhand:ismenzen() and not myhand:ready()
    local feed = who == firstriichi and game:riichiestablished(self)
    if catch or feed then
        accelerate(mount, myhand)
    end
  end
end

function tomobiki(mount, game, who)
  local myhand = game:gethand(self)

  if who == mSelf and #myHand:barks() == 4 then
    accelerate(mount, myhand)
  end

  -- feeding, not so precise
  if not myhand:ismenzen() and who ~= mSelf then
    local giftset = {}
    for _, t in ipairs(game:gethand(who):effa()) do
      giftset[t:id34()] = true
    end

    for _, t in ipairs(myhand:effa4()) do
      if myhand:canpon(t) then
        if not giftset[t:id34()] then
          mount.lightA(t, 1000);
        end
      end
    end
  end
end

function accelerate(mount, hand)
  local effaset = {}
  for _, t in ipairs(hand:effa()) do
    effaset[t:id34()] = true
  end

  for _, t in ipairs(T34.all) do
    local mk = effaset[t:id34()] and 1400 or -100
    mount:lighta(t, mk)
    mount:lightb(t, mk / 10)
  end
end
