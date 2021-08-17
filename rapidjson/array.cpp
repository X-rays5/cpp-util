/*
	when given a array like this "imaarray":[-328.78125,10.18750,-337.34375]
	the method to extract values is
*/
	auto& array = json["imaarray"];
	for (rapidjson::SizeType i = 0; i< a.Size(); i++)
	{
    	float v = array[i].GetFloat();
	}

	// or access manually
	float v = array[0].GetFloat();

/*
	array iterator used be arrays like below
	"Items":[
    { "id":128049382, "Name":"hpt_pulselaser_fixed_medium", "BuyPrice":16731 },
    { "id":128049383, "Name":"hpt_pulselaser_fixed_large", "BuyPrice":66924 },
	]
*/
if (json.HasMember("Items")) {
	auto Items = json["Items"].GetArray();

    std::vector<items> ItemEntries;
    for (rapidjson::Value::ConstValueIterator itr = Items.Begin(); itr != Items.End(); ++itr) {
    	item i;

    	if ((*itr).HasMember("Name")) {
        	i.Name = (*itr)["Name"].GetString();
            ItemEntries.emplace_back(i);
		}
	}
}
