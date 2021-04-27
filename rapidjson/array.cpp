/*
	when given a array like this "imaarray":[-328.78125,10.18750,-337.34375]
	the method to extract values is
	auto& array = json["imaarray"];
	for (rapidjson::SizeType i = 0; i<a.Size(); i++)
	{
    float v = array[i].GetFloat();
	}
	or access manually
	float v = array[0].GetFloat();
*/