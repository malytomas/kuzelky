#define _CRT_SECURE_NO_WARNINGS 1

#include <bits/stdc++.h>
#include <unordered_map>
#include <array>

using namespace std;

struct Config
{
	struct BitRef
	{
		uint64_t &u;
		const uint64_t b;

		BitRef(Config &config, int index) : u(config.config[index / 64]), b(uint64_t(1) << (index % 64))
		{}

		void operator = (bool value)
		{
			if (value)
				u = u | b;
			else
				u = u & ~b;
		}

		operator bool () const
		{
			return (u & b) != 0;
		}

		void flip()
		{
			u = u ^ b;
		}
	};

	array<uint64_t, 5> config;

	Config()
	{
		for (int i = 0; i < 5; i++)
			config[i] = 0;
	}

	BitRef operator [] (int index)
	{
		return BitRef(*this, index);
	}

	void flip(int index)
	{
		(*this)[index].flip();
	}

	bool operator == (const Config &other) const
	{
		for (int i = 0; i < 5; i++)
			if (config[i] != other.config[i])
				return false;
		return true;
	}

	bool operator < (const Config &other) const
	{
		return std::tie(config[0], config[1], config[2], config[3], config[4])
			< std::tie(other.config[0], other.config[1], other.config[2], other.config[3], other.config[4]);
	}
};

namespace std
{
	template<> struct hash<Config>
	{
		std::size_t operator()(const Config &c) const noexcept
		{
			const auto &a = c.config;
			std::hash<uint64_t> h;
			return h(h(h(h(a[0]) + a[1]) + a[2]) + a[3]) + a[4];
		}
	};
}

struct Cache
{
	void check(const Config &config, bool &valid, bool &result)
	{
		auto it = data.find(config);
		if (it == data.end())
		{
			valid = false;
			return;
		}
		valid = true;
		result = it->second;
	}

	bool set(const Config &config, bool result)
	{
		data[config] = result;
		return result;
	}

	Cache()
	{
		data.reserve(1000000);
		{
			Config c;
			set(c, false); // ""
			c.config[0] = 1;
			set(c, true); // "I"
			c.config[0] = 2;
			set(c, true); // "II"
		}
	}

private:
	std::unordered_map<Config, bool> data;
} cache;

bool winning(Config config)
{
	// check cache
	{
		bool valid, result;
		cache.check(config, valid, result);
		if (valid)
			return result;
	}

	// recurse
	for (int segment = 1; segment < 300; segment++)
	{
		if (!config[segment])
			continue;
		// try to shoot one pin
		for (int i = 0; i < segment / 2 + 1; i++)
		{
			Config c(config);
			c[segment].flip();
			c[segment - i - 1].flip();
			c[i].flip();
			c[0] = false;
			if (!winning(c))
				return cache.set(config, true);
		}
		if (segment > 1)
		{
			// try to shoot two pins
			for (int i = 0; i < segment / 2; i++)
			{
				Config c(config);
				c[segment].flip();
				c[segment - i - 2].flip();
				c[i].flip();
				c[0] = false;
				if (!winning(c))
					return cache.set(config, true);
			}
		}
	}

	return cache.set(config, false);
}

// stupid strings
string isWinning(int, const string &config)
{
	Config cc;
	int last = 0;
	for (char c : config)
	{
		if (c == 'I')
			last++;
		else
		{
			if (last)
				cc[last].flip();
			last = 0;
		}
	}
	if (last)
		cc[last].flip();
	return winning(cc) ? "WIN" : "LOSE";
}

int main()
{
	ofstream fout(getenv("OUTPUT_PATH"));
	int t;
	cin >> t;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	for (int t_itr = 0; t_itr < t; t_itr++)
	{
		int n;
		cin >> n;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		string config;
		getline(cin, config);
		string result = isWinning(n, config);
		fout << result << "\n";
	}
	fout.close();
	return 0;
}
