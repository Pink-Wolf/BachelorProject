namespace CompWolf.Doc.Server.Models
{
    public class OverloadApiModel
    {
        public string Declaration { get; set; }
        public string Description { get; set; }
    }
    public class FunctionApiModel
    {
        public string Name { get; set; }
        public string BriefDescription { get; set; }

        public string[] Warnings { get; set; }
        public string[] Exceptions { get; set; }

        public OverloadApiModel[] Overloads { get; set; }

        public string[] Related { get; set; }
    }
}
