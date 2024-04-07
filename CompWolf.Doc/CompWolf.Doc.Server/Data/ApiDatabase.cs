namespace CompWolf.Doc.Server.Data
{
    public class ApiDatabase
    {
        public async Task<string?> GetEntity(string project, string header, string name)
        {
            var path = $"data/{project}/{header}/{name}.json";
            if (File.Exists(path) is false) return null;
            return await File.ReadAllTextAsync(path);
        }
    }
}
