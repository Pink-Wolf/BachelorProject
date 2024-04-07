namespace CompWolf.Doc.Server.Models
{
    public class DataMemberApiModel
    {
        public string Name { get; set; }
        public string Description { get; set; }
    }
    public class MethodGroupApiModel
    {
        public string Name { get; set; }
        public string Description { get; set; }
        public FunctionApiModel[] Items { get; set; }
    }

    public class ClassApiModel
    {
        public string Namespace { get; set; }
        public string Declaration { get; set; }
        public string BriefDescription { get; set; }
        public string DetailedDescription { get; set; }

        public string[] Warnings { get; set; }

        public DataMemberApiModel[] DataMembers { get; set; }
        public FunctionApiModel Constructor { get; set; }
        public MethodGroupApiModel[] MethodGroups { get; set; }

        public ExampleApiModel Example { get; set; }

        public string[] Related { get; set; }
    }
}
