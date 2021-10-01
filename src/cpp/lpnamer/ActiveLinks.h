
#ifndef ANTARESXPANSION_ACTIVELINKS_H
#define ANTARESXPANSION_ACTIVELINKS_H

#include <Candidate.h>
#include <unordered_map>

using LinkName = std::string;

class ActiveLink {

public:
	ActiveLink(int idLink, const std::string linkName);
	void setAlreadyInstalledLinkProfile(const LinkProfile& linkProfile);

	void addCandidate(const CandidateData& candidate_data, const LinkProfile& candidate_profile);
	const std::vector<Candidate>& getCandidates() const;
	
	double already_installed_direct_profile(size_t timeStep) const;
	double already_installed_indirect_profile(size_t timeStep) const;

public:
	int _idLink;
	LinkName _name;
	double _already_installed_capacity;
	std::string _linkor;
	std::string _linkex;

private:
	LinkProfile _already_installed_profile;
	std::vector<Candidate> _candidates;
};

class ActiveLinksBuilder {

public:

    ActiveLinksBuilder(const std::vector<CandidateData>& candidateList, const std::map<std::string, LinkProfile>& profile_map);
	
	const std::vector<ActiveLink>& getLinks();

private:
	struct LinkData {
		int id;
		double installed_capacity;
		std::string profile_name;
		std::string _linkor;
		std::string _linkex;
	};

	void checkCandidateNameDuplication();
	void checkLinksValidity();

	int getLinkIndexOf(int link_id) const;
	void addCandidate(const CandidateData &candidate_data);
	void launchExceptionIfNoLinkProfileAssociated(const std::string& profileName);

	void record_link_data(const CandidateData& candidateData);
	void raise_errors_if_link_data_differs_from_existing_link(const LinkData& link_data,
		const LinkName& link_name) const;
	void create_links();

	LinkProfile getProfileFromProfileMap(const std::string& profile_name) const;

	std::map<LinkName, LinkData> _links_data;
    std::unordered_map<LinkName, std::string> linkToAlreadyInstalledProfileName;
	std::unordered_map<LinkName, double> linkToAlreadyInstalledCapacity;
	const std::vector<CandidateData> _candidateDatas;
	const std::map<std::string, LinkProfile> _profile_map;
    std::vector <ActiveLink> _links;

    

    
};

#endif //ANTARESXPANSION_ACTIVELINKS_H