import { Reference } from "../CodeComponents";
import FormattedText from "../FormattedText";

export default function ProjectListViewer({ data }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<section>
			<h2>Projects:</h2>
			<ul>
				{data.projects.map((x, i) => {
					return (
						<li key={i}>
							<Reference path={`/api/${x.name}`}>{x.name}</Reference>: <FormattedText>{x.briefDescription}</FormattedText>
						</li>
					)
				})}
			</ul>
		</section>
	)
}