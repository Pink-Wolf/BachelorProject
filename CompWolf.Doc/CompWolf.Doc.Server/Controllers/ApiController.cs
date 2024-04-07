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

        [HttpGet("{project}/{header}/{name}")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public virtual async Task<ActionResult<ClassApiModel>> GetAsync(
            [FromRoute] string project,
            [FromRoute] string header,
            [FromRoute] string name)
        {
            var output = Database.GetEntity(project, header, name);
            if (output is null) return NotFound();
            return output;
        }
    }
}
