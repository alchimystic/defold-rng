local M = {}

function M.pcg32(stat, inc)
	if stat == nil then
		return rng.pcg32()
	else
		return rng.pcg32(
		assert(tonumber(stat), "stat is not a number"),
		assert(tonumber(inc), "inc is not a number")
	)
end
end

function M.tinymt32(seed)
return rng.tinymt32(assert(tonumber(seed), "seed is not a number"))
end

return M
