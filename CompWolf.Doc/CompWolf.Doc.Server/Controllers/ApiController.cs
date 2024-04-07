using CompWolf.Doc.Server.Data;
using CompWolf.Doc.Server.Models;
using Microsoft.AspNetCore.Mvc;

namespace CompWolf.Doc.Server.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class ApiController : ControllerBase
    {
        public ApiDatabase Database = new();

        [HttpGet("overview")]
        [ProducesResponseType(200)]
        public virtual ActionResult<ApiCollection> GetOverview() => Database.GetOverview();

        [HttpGet("{project}/{header}/{name}")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public virtual async Task<ActionResult<string>> GetEntityAsync(
            [FromRoute] string project,
            [FromRoute] string header,
            [FromRoute] string name)
        {
            var output = await Database.GetEntityAsync(project, header, name);
            if (output is null) return NotFound();
            return output;
        }
    }
}
