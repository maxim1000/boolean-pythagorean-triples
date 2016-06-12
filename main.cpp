#include <chrono>
#include <iostream>
#include <vector>
enum class TColor
{
	Black,
	White,
	Undefined
};
struct TTriplet
{
	int Elements[3];
};
std::vector<TTriplet> FindTriplets(int max)
{
	std::vector<TTriplet> triplets;
	for(int a=1;a<=max;++a)
	{
		for(int b=a+1;b<=max;++b)
		{
			for(int c=b+1;c<=max;++c)
			{
				if(a*a+b*b==c*c)
					triplets.push_back({a,b,c});
			}
		}
	}
	return triplets;
}
bool IsCorrect(
	const std::vector<TColor> &colors,
	const std::vector<TTriplet> &triplets)
{
	for(const auto &triplet:triplets)
	{
		bool hasWhite=false;
		bool hasBlack=false;
		bool hasUndefined=false;
		for(auto element:triplet.Elements)
		{
			if(colors[element]==TColor::Black)
				hasBlack=true;
			if(colors[element]==TColor::White)
				hasWhite=true;
			if(colors[element]==TColor::Undefined)
				hasUndefined=true;
		}
		if(!hasUndefined)
		{
			if(!hasBlack||!hasWhite)
				return false;
		}
	}
	return true;
}
bool IsPossible(
	std::vector<TColor> &colors,
	const std::vector<TTriplet> &triplets,
	const std::vector<std::vector<TTriplet>> &tripletMap)
{
	const auto undefined=std::find(colors.begin(),colors.end(),TColor::Undefined);
	if(undefined==colors.end())
		return true;
	const int element=int(undefined-colors.begin());
	for(auto newColor:{TColor::Black,TColor::White})
	{
		*undefined=newColor;
		if(IsCorrect(colors,tripletMap[element]))
		{
			if(IsPossible(colors,triplets,tripletMap))
				return true;
		}
	}
	*undefined=TColor::Undefined;
	return false;
}
bool IsPossible(int max)
{
	std::vector<TColor> colors(max+1,TColor::Undefined);
	const auto triplets=FindTriplets(max);
	std::vector<std::vector<TTriplet>> tripletMap(max+1);
	for(const auto &triplet:triplets)
	{
		for(int element:triplet.Elements)
			tripletMap[element].push_back(triplet);
	}
	if(IsPossible(colors,triplets,tripletMap))
	{
		const bool hasUndefined=
			std::find(colors.begin(),colors.end(),TColor::Undefined)!=colors.end();
		if(hasUndefined||!IsCorrect(colors,triplets))
			throw std::logic_error("found an incorrect coloring");
		return true;
	}
	else
		return false;
}
int main()
{
	try
	{
		for(int max=205;max<8000;++max)
		{
			const auto start=std::chrono::steady_clock::now();
			std::cout<<max<<": ";
			std::cout<<IsPossible(max)?"possible":"impossible";
			const auto finish=std::chrono::steady_clock::now();
			const auto ms=
				std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count();
			std::cout<<", "<<ms/1000.<<"s"<<std::endl;
		}
	}
	catch(const std::exception &e)
	{
		std::cerr<<e.what()<<std::endl;
		return 1;
	}
	return 0;
}
