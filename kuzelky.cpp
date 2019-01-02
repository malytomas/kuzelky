#include <bits/stdc++.h>
#include <array>

using namespace std;

struct Config
{
	array<uint8_t, 300> config;
	uint16_t sum;

	Config() : sum(0)
	{
		for (uint8_t &c : config)
			c = 0;
	}

	uint8_t &operator [] (int index)
	{
		return config[index];
	}

	void print() const
	{
		for (uint8_t c : config)
		{
			if (c)
			{
				for (int i = 0; i < c; i++)
					cout << 'I';
				cout << '_';
			}
		}
		cout << endl;
	}

	bool valid() const
	{
		return sum == computeSum()
			&& config[0] == 0;
	}

	void validate() const
	{
		assert(valid());
	}

	uint16_t computeSum() const
	{
		uint16_t sum = 0;
		uint16_t l = 0;
		for (uint8_t c : config)
		{
			sum += c * l;
			l++;
		}
		return sum;
	}

	void optimize()
	{
		{
			// even segments of length 1 do not change winning status
			//uint8_t &c = config[1];
			//int s = c / 2 * 2;
			//c -= s;
			//sum -= s;
			for (int i = 0; i < 300; i++)
				config[i] %= 2;
			sum = computeSum();
		}
		validate();
	}

	bool operator < (const Config &other) const
	{
		validate();
		other.validate();
		if (sum < other.sum)
			return true;
		if (other.sum < sum)
			return false;
		assert(sum == other.sum);
		return memcmp(config.data(), other.config.data(), config.size()) < 0;
	}
};

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

private:
	map<Config, bool> data;
} cache;

bool winning(Config config)
{
	config.optimize();

	// check cache
	{
		bool valid, result;
		cache.check(config, valid, result);
		if (valid)
			return result;
	}

	// check trivial winning conditions
	if (config.sum < 3)
	{
		if (config.sum == 0)
			return cache.set(config, false); // ""
		if (config.sum == 1)
			return cache.set(config, true); // "I"
		if (config[2] == 1)
			return cache.set(config, true); // "II"
		return cache.set(config, false); // "I_I"
	}

	// recurse
	for (uint16_t segment = 1; segment < config.config.size(); segment++)
	{
		if (!config[segment])
			continue;
		// try to shoot one pin
		for (int i = 0; i < segment / 2 + 1; i++)
		{
			Config c(config);
			c[segment]--;
			c[segment - i - 1]++;
			c[i]++;
			c[0] = 0;
			c.sum--;
			if (!winning(c))
				return cache.set(config, true);
		}
		if (segment > 1)
		{
			// try to shoot two pins
			for (int i = 0; i < segment / 2; i++)
			{
				Config c(config);
				c[segment]--;
				c[segment - i - 2]++;
				c[i]++;
				c[0] = 0;
				c.sum -= 2;
				if (!winning(c))
					return cache.set(config, true);
			}
		}
	}

	return cache.set(config, false);
}

// stupid strings
string isWinning(int, string config)
{
	Config cc;
	uint8_t last = 0;
	for (char c : config)
	{
		if (c == 'I')
			last++;
		else
		{
			if (last)
				cc.config[last]++;
			last = 0;
		}
	}
	if (last)
		cc.config[last]++;
	cc.sum = cc.computeSum();
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
