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
	std::vector<int> roots(2*max*max+1,-1);
	for(int c=0;c*c<int(roots.size());++c)
		roots[c*c]=c;
	std::vector<TTriplet> triplets;
	for(int a=1;a<=max;++a)
	{
		for(int b=a+1;b<=max;++b)
		{
			const int c=roots[a*a+b*b];
			if(c!=-1 && c<=max)
				triplets.push_back({a,b,c});
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
bool IsCorrect(
	const std::vector<TColor> &colors,
	const std::vector<std::vector<TTriplet>> &tripletMap,
	const std::vector<int> &elementSubset)
{
	for(int element:elementSubset)
	{
		if(!IsCorrect(colors,tripletMap[element]))
			return false;
	}
	return true;
}
TColor GetOppositeColor(TColor color)
{
	switch(color)
	{
		case TColor::Black:
			return TColor::White;
		case TColor::White:
			return TColor::Black;
		case TColor::Undefined:
			return TColor::Undefined;
		default:
			throw std::logic_error("unexpected value of TColor");
	}
}
std::vector<int> UpdateElements(
	std::vector<TColor> &colors,
	const std::vector<std::vector<TTriplet>> &tripletMap,
	int notUpdatedElement)
{
	std::vector<int> updatedElements;
	updatedElements.push_back(notUpdatedElement);
	for(int index=0;index<int(updatedElements.size());++index)
	{
		for(const auto &triplet:tripletMap[updatedElements[index]])
		{
			for(int shift=0;shift<3;++shift)
			{
				int shifted[3];
				for(int c=0;c<3;++c)
					shifted[c]=triplet.Elements[(shift+c)%3];
				if(colors[shifted[0]]!=colors[shifted[1]])
					continue;
				if(colors[shifted[0]]==TColor::Undefined)
					continue;
				if(colors[shifted[2]]==TColor::Undefined)
				{
					colors[shifted[2]]=GetOppositeColor(colors[shifted[0]]);
					updatedElements.push_back(shifted[2]);
				}
			}
		}
	}
	updatedElements.erase(updatedElements.begin());
	return updatedElements;
}
bool IsPossible(
	std::vector<TColor> &colors,
	const std::vector<TTriplet> &triplets,
	const std::vector<std::vector<TTriplet>> &tripletMap)
{
	int nextElement=-1;
	for(int element=0;element<int(tripletMap.size());++element)
	{
		if(colors[element]!=TColor::Undefined)
			continue;
		if(nextElement==-1 || tripletMap[element].size()>tripletMap[nextElement].size())
			nextElement=element;
	}
	if(nextElement==-1)
		return true;
	for(auto newColor:{TColor::Black,TColor::White})
	{
		colors[nextElement]=newColor;
		auto updatedElements=UpdateElements(colors,tripletMap,nextElement);
		updatedElements.push_back(nextElement);
		if(IsCorrect(colors,tripletMap,updatedElements))
		{
			if(IsPossible(colors,triplets,tripletMap))
				return true;
		}
		for(int element:updatedElements)
			colors[element]=TColor::Undefined;
	}
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
		for(int max=3140;max<8000;++max)
		{
			const auto start=std::chrono::steady_clock::now();
			std::cout<<max<<": ";
			std::cout<<(IsPossible(max)?"possible":"impossible");
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
