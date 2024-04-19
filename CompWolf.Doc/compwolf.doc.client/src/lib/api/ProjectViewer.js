import { Reference } from "../CodeComponents";
import FormattedText from "../FormattedText";

export default function ProjectViewer({ data }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<section>
			<h1 id="Declaration">
				{`${data.project}`}
			</h1>
			<p id="Description">
				<FormattedText>{data.detailedDescription}</FormattedText>
			</p>
			<h2>Header-files:</h2>
			<ul>
				{data.headers.map((x, i) => {
					return (
						<li key={i}>
							<Reference path={`${data.project}/${x.name}`}>{x.name}</Reference>: <FormattedText>{x.briefDescription}</FormattedText>
						</li>
					)
				})}
			</ul>
		</section>
	)
}